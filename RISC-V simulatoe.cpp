#include<iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
using namespace std;

int PC; // Program Counter
int initialPC;
int reg[32]; // Array of 32 registers
vector<pair<int, string>> memory; // Memory representation (not used in this code)
vector<string> instruction; // Vector to store instructions

void driversprogram() {
    cout << "Please enter the starting address of the program: ";
    cin >> PC; // Set the starting address
    initialPC = PC;
    for (int i = 0; i < 32; i++) {
        reg[i] = 0; // Initialize registers to 0
    }
}

void read_input_from_file(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    int i = 0;
    int increment = 0;
    while (getline(inputFile, line)) {
        if (line == "HALT") { // Stop if "HALT" is encountered
            break;
        }
        instruction.push_back(line); // Store the instruction
        pair<int, string> x (PC + increment, line);
        memory.push_back(x);
        increment += 4;
        i++;
    }
    for (int i = 0; i < memory.size(); i++)
    {
        cout << memory[i].first << " " << memory[i].second << endl;
    }
    inputFile.close(); // Close the file
}

void simulator(int& programcount) {
    string action;
    int i = 0;
    //every iteration in the loop one must change i and PC and size according to the 
    // instruction that is currently executed
    while(true){
        action = "";
        string rd = "";
        string imm = "";

        // Extract the action (instruction) part
        for (int j = 0; j < instruction[i].size(); j++) {
            if (instruction[i][j] == ' ') {
                break;
            }
            action += instruction[i][j];
        }

        cout << action << endl;
        if (action == "ecall")
        {
            return;
        }

        if (action == "LUI") {
            int rdIndex;
            int immediateValue;
            PC = PC + 4;
            

            // Parse `rd` and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                imm = instruction[i].substr(instruction[i].find(",") + 1);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value

            // Perform LUI operation (load upper immediate)
            reg[rdIndex] = immediateValue * 4096; // Store the immediate value multiplied by 4096

            cout << "LUI: Loaded " << (immediateValue * 4096) << " into register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            cout << rdIndex << endl;
            cout << reg[rdIndex] << endl;
            i++;
        }
        else if (action == "AUIPC")
        {
            int rdIndex;
            int immediateValue;
            // Parse `rd` and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                imm = instruction[i].substr(instruction[i].find(",") + 1);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value

            // Perform AUIPC operation (add immediate to PC and store in rd)
            reg[rdIndex] = PC + (immediateValue << 12); // Store the PC value + (imm << 12) in register

            cout << "AUIPC: Loaded " << reg[rdIndex] << " into register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            i++;

        }
        else if (action == "JAL") 
        {
            int rdIndex;
            int immediateValue;
           
            PC = PC + 4;
            // Parse `rd` and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                imm = instruction[i].substr(instruction[i].find(",") + 1);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `imm` (string) to an integer directly as decimal
            cout << imm << endl;
            immediateValue = stoi(imm); // Treat the input as a decimal value

            cout << immediateValue;
            int nextPC = PC + 4;
            PC += immediateValue;
            cout << PC << endl;

            // Save the return address in rd
            reg[rdIndex] = nextPC;

            
            cout << "JAL: Jumping to address " << PC << " and link to register " << rd << " with return address " << nextPC << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            i = i+immediateValue/4 ;
        }
        else if (action == "JALR")
        {
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse `rd`, `rs1`, and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                string X= instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);
                if (X.length() == 3)
                {
                    X = X.erase(0, 1);
                }
                rs1Index = stoi(X);

                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index];

            // Update PC to the address in rs1 plus immediate value
            PC = reg[rs1Index] + immediateValue;
            cout << "PC: " << PC << endl;

            cout << "JALR: Jumping to address " << PC << " from register " << "x" << rs1Index << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            //exit(0);
            // Update loop index based on new PC value
            i = (PC - initialPC) / 4 ;
        }


        // Add additional instructions here as needed
        else {
            cout << "Unknown instruction: " << action << endl;
        }
        cout << endl;
        cout << "I" << i << endl;
        cout << "PC" << PC << endl;
        cout << endl;
    }
}

int main() {
    // Start of the driver program
    driversprogram();
    // End of the driver program

    // Read the RISC-V code from a file
    string filename = "C:/Users/HP/OneDrive/Desktop/RISC-V.txt";
    /*cout << "Please enter the filename to read instructions from: ";
    cin >> filename;*/ // Get filename from user
    read_input_from_file(filename);

    // Start computing the RISC-V code
    simulator(PC);
    // End of computing the RISC-V code

    return 0;
}


