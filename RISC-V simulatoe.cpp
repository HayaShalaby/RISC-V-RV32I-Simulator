#include<iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <iomanip>
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
        if(line[0] == '#')
            continue;
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

// Finding the value at memory address 'first'
string memFind(int first){
    for(int i = 0; i < memory.size(); i++){
        if(memory[i].first == first)
            return memory[i].second;
    }
}

// Writing the 'second' value to memory at address 'first'
void memWrite(int first, string second){
    for(int i = 0; i < memory.size(); i++){
        if(memory[i].first == first)
            memory[i].second = second;
    }
}

// Formatted simulation output
void simDisplay(){
    // Output PC
    std::cout << "Program Counter: " << PC << "\n\n";

    // Output RF
    std::cout << "Register File:" << std::endl;
    for (int i = 0; i < 32; ++i) {
        std::cout << "  Register [" << i << "]: " << std::setw(4) << reg[i] << std::endl;
    }
    std::cout << std::endl;

    // Output Memory
    std::cout << "Memory:" << std::endl;
    for (size_t i = 0; i < memory.size(); ++i) {
        std::cout << "  Location [" << i << "]: "
                  << std::setw(4) << memory[i].first
                  << " -> " << std::setw(10) << memory[i].second << std::endl;
    }
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
        string rs2 = "";
        string rs1 = "";

        // Extract the action (instruction) part
        for (int j = 0; j < instruction[i].size(); j++) {
            if (instruction[i][j] == ' ') {
                break;
            }
            action += instruction[i][j];
        }

        cout << action << endl;
        if (action == "ECALL" or action == "EBREAK" or action == "PAUSE" or action == "FENCE" or action == "FENCE.TSO")
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

        else if (action == "LI") {
            int rdIndex;           // Register to store the immediate value
            int immediate;         // The immediate value to load

            // Parse rd and immediate from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != std::string::npos) {
                // Parsing `rd` by locating the first space and comma characters
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Parsing immediate value (everything after the comma)
                size_t pos2 = instruction[i].find(",");
                std::string immStr = instruction[i].substr(pos2 + 1);

                // Convert `immediate` to an integer directly (assuming decimal)
                immediate = std::stoi(immStr);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = std::stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Write the immediate value to the register array at the `rdIndex`
            reg[rdIndex] = immediate;
            std::cout << "Loaded immediate " << immediate << " into register x" << rdIndex << std::endl;

            // Increment program counter and instruction index to point to the next instruction
            i++;
            PC += 4;
        }


            // Load half-word unsigned
        else if(action == "LHU"){
            int rdIndex, rs1Index;
            int offset;

            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                // Parsing rd by locating the first space character in the string
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Parsing offset
                size_t pos2 = instruction[i].find(",");
                imm = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2) - pos2 - 1);

                // Parsing rs1 by locating the first open bracket in the string and extracting the register
                size_t pos3 = instruction[i].find("(");
                string X = instruction[i].substr(pos3 + 1, instruction[i].find(")") - pos3 - 1).substr(1);
                if (X.length() == 3)
                {
                    X = X.erase(0, 1);
                }
                rs1Index = stoi(X);
            }

            // Convert `rd` to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `offset` (string) to an integer directly as decimal
            offset = stoi(imm); // Treat the input as a decimal value

            // Getting the value desired in memory at the memory address rs1 + offset
            string loadValue = memFind(reg[rs1Index] + offset);

            // Get LS 16 bits from the load value
            int loadHW = stoi(loadValue) & 0xFFFF;

            // Loading the value into memory address at rd
            memWrite(reg[rdIndex], to_string(loadHW));
            cout << "Value " << loadHW << " written in memeory address " << reg[rdIndex] <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "SB"){
            int rdIndex, rs1Index;
            int offset;

            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                // Parsing rs1 by locating the first space character in the string
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Parsing offset
                size_t pos2 = instruction[i].find(",");
                imm = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2) - pos2 - 1);

                // Parsing rd by locating the first open bracket in the string and extracting the register
                size_t pos3 = instruction[i].find("(");
                string rd = instruction[i].substr(pos3 + 1, instruction[i].find(")") - pos3 - 1).substr(1);
                if (rd.length() == 3)
                {
                    rd = rd.erase(0, 1);
                }
                rdIndex = stoi(rd);
            }

            // Convert `rs1` to register index (assuming rs1 is in the form "x0", "x1", etc.)
            rs1Index = stoi(rs1.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `offset` (string) to an integer directly as decimal
            offset = stoi(imm); // Treat the input as a decimal value

            // Getting the LS byte from register value
            int byte = reg[rs1Index] & 0xFF;

            // Storing the value into memory address at rd
            memWrite(reg[rdIndex + offset], to_string(byte));
            cout << "Value " << byte << " stored at memory address " << reg[rdIndex + offset] <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "SH"){
            int rdIndex, rs1Index;
            int offset;

            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                // Parsing rs1 by locating the first space character in the string
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Parsing offset
                size_t pos2 = instruction[i].find(",");
                imm = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2) - pos2 - 1);

                // Parsing rd by locating the first open bracket in the string and extracting the register
                size_t pos3 = instruction[i].find("(");
                string rd = instruction[i].substr(pos3 + 1, instruction[i].find(")") - pos3 - 1).substr(1);
                if (rd.length() == 3)
                {
                    rd = rd.erase(0, 1);
                }
                rdIndex = stoi(rd);
            }

            // Convert `rs1` to register index (assuming rs1 is in the form "x0", "x1", etc.)
            rs1Index = stoi(rs1.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `offset` (string) to an integer directly as decimal
            offset = stoi(imm); // Treat the input as a decimal value

            // Getting the LS halfword from register value
            int hw = reg[rs1Index] & 0xFFFF;

            // Storing the value into memory address at rd
            memWrite(reg[rdIndex + offset], to_string(hw));
            cout << "Value " << hw << " stored at memory address " << reg[rdIndex + offset] <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "SW"){
            int rdIndex, rs1Index;
            int offset;

            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                // Parsing rs1 by locating the first space character in the string
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Parsing offset
                size_t pos2 = instruction[i].find(",");
                imm = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2) - pos2 - 1);

                // Parsing rd by locating the first open bracket in the string and extracting the register
                size_t pos3 = instruction[i].find("(");
                string rd = instruction[i].substr(pos3 + 1, instruction[i].find(")") - pos3 - 1).substr(1);
                if (rd.length() == 3)
                {
                    rd = rd.erase(0, 1);
                }
                rdIndex = stoi(rd);
            }

            // Convert `rs1` to register index (assuming rs1 is in the form "x0", "x1", etc.)
            rs1Index = stoi(rs1.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert `offset` (string) to an integer directly as decimal
            offset = stoi(imm); // Treat the input as a decimal value

            // Storing the value into memory address at rd
            memWrite(reg[rdIndex + offset], to_string(reg[rs1Index]));
            cout << "Value " << reg[rs1Index] << " stored at memory address " << reg[rdIndex + offset] <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "ADDI"){
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Only erase the first character if it's actually "x"
            if (rs1[0] == 'x') {
                rs1 = rs1.erase(0, 1);
            }

            if (rd[0] == 'x') {
                rd = rd.erase(0, 1);
            }

            rs1Index = stoi(rs1);

            rdIndex = stoi(rd);

            if(rdIndex == 0){
                cout << "WARNING: Attempt to manipulate zero register -- rejected." <<endl;
                continue;
            }

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] <<endl;

            // Perform the calculation
            reg[rdIndex] = reg[rs1Index] + immediateValue;
            cout << "Value " << reg[rdIndex] << " written in register " << rdIndex <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "SLTI"){
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Only erase the first character if it's actually "x"
            if (rs1[0] == 'x') {
                rs1 = rs1.erase(0, 1);
            }

            if (rd[0] == 'x') {
                rd = rd.erase(0, 1);
            }

            rs1Index = stoi(rs1);

            rdIndex = stoi(rd);

            if(rdIndex == 0){
                cout << "WARNING: Attempt to manipulate zero register -- rejected." <<endl;
                continue;
            }

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] <<endl;

            // Perform the calculation
            if(reg[rs1Index] < immediateValue)
                reg[rdIndex] = 1;
            else
                reg[rdIndex] = 0;

            cout << "Register " << rdIndex << " set to " << reg[rdIndex] <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "SLTIU"){
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Only erase the first character if it's actually "x"
            if (rs1[0] == 'x') {
                rs1 = rs1.erase(0, 1);
            }

            if (rd[0] == 'x') {
                rd = rd.erase(0, 1);
            }

            rs1Index = stoi(rs1);

            rdIndex = stoi(rd);

            if(rdIndex == 0){
                cout << "WARNING: Attempt to manipulate zero register -- rejected." <<endl;
                continue;
            }

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] <<endl;

            // Perform the calculation
            if(abs(reg[rs1Index]) < abs(immediateValue))
                reg[rdIndex] = 1;
            else
                reg[rdIndex] = 0;

            cout << "Register " << rdIndex << " set to " << reg[rdIndex] <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "XORI"){
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Only erase the first character if it's actually "x"
            if (rs1[0] == 'x') {
                rs1 = rs1.erase(0, 1);
            }

            if (rd[0] == 'x') {
                rd = rd.erase(0, 1);
            }

            rs1Index = stoi(rs1);

            rdIndex = stoi(rd);

            if(rdIndex == 0){
                cout << "WARNING: Attempt to manipulate zero register -- rejected." <<endl;
                continue;
            }

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] <<endl;

            // Perform the calculation
            reg[rdIndex] = reg[rs1Index] ^ immediateValue;
            cout << "Value " << reg[rdIndex] << " written in register " << rdIndex <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "ORI"){
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Only erase the first character if it's actually "x"
            if (rs1[0] == 'x') {
                rs1 = rs1.erase(0, 1);
            }

            if (rd[0] == 'x') {
                rd = rd.erase(0, 1);
            }

            rs1Index = stoi(rs1);

            rdIndex = stoi(rd);

            if(rdIndex == 0){
                cout << "WARNING: Attempt to manipulate zero register -- rejected." <<endl;
                continue;
            }

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] <<endl;

            // Perform the calculation
            reg[rdIndex] = reg[rs1Index] | immediateValue;
            cout << "Value " << reg[rdIndex] << " written in register " << rdIndex <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "ANDI"){
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Only erase the first character if it's actually "x"
            if (rs1[0] == 'x') {
                rs1 = rs1.erase(0, 1);
            }

            if (rd[0] == 'x') {
                rd = rd.erase(0, 1);
            }

            rs1Index = stoi(rs1);

            rdIndex = stoi(rd);

            if(rdIndex == 0){
                cout << "WARNING: Attempt to manipulate zero register -- rejected." <<endl;
                continue;
            }

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] <<endl;

            // Perform the calculation
            reg[rdIndex] = reg[rs1Index] & immediateValue;
            cout << "Value " << reg[rdIndex] << " written in register " << rdIndex <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "SLLI"){
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Only erase the first character if it's actually "x"
            if (rs1[0] == 'x') {
                rs1 = rs1.erase(0, 1);
            }

            if (rd[0] == 'x') {
                rd = rd.erase(0, 1);
            }

            rs1Index = stoi(rs1);

            rdIndex = stoi(rd);

            if(rdIndex == 0){
                cout << "WARNING: Attempt to manipulate zero register -- rejected." <<endl;
                continue;
            }

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] <<endl;

            // Perform the calculation
            reg[rdIndex] = reg[rs1Index] * (pow(2, immediateValue));
            cout << "Value " << reg[rdIndex] << " written in register " << rdIndex <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }

        else if(action == "SRLI"){
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find `rs1` by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract `imm` after the last comma
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
            }

            // Only erase the first character if it's actually "x"
            if (rs1[0] == 'x') {
                rs1 = rs1.erase(0, 1);
            }

            if (rd[0] == 'x') {
                rd = rd.erase(0, 1);
            }

            rs1Index = stoi(rs1);

            rdIndex = stoi(rd);

            if(rdIndex == 0){
                cout << "WARNING: Attempt to manipulate zero register -- rejected." <<endl;
                continue;
            }

            // Convert `imm` (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] <<endl;

            // Perform the calculation
            reg[rdIndex] = reg[rs1Index] / (pow(2, immediateValue));
            cout << "Value " << reg[rdIndex] << " written in register " << rdIndex <<endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
        }



        // Add additional instructions here as needed
        else {
            cout << "Unknown instruction: " << action << endl;
        }
        cout << endl;

        cout << "Main Loop Index: " << i << endl;

        simDisplay();

        cout << "-----------------------------------------------------------------------" <<endl;
    }
}

int main() {
    // Start of the driver program
    driversprogram();
    // End of the driver program

    // Read the RISC-V code from a file
    string filename = "C:/Users/Haya/Desktop/Assembly Project1/RISC-V-RV32I-Simulator/Test Program 3.txt";
    /*cout << "Please enter the filename to read instructions from: ";
    cin >> filename;*/ // Get filename from user
    read_input_from_file(filename);

    // Start computing the RISC-V code
    simulator(PC);
    // End of computing the RISC-V code

    return 0;
}


