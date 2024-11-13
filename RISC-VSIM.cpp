#include<iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <iomanip>
using namespace std;

int PC; // Program Counter
int initialPC;
int reg[32]; // Array of 32 registers
vector<pair<int, int>> memory; // Memory representation (not used in this code)
vector<string> instruction; // Vector to store instructions
void driversprogram() {
    cout << "Please enter the starting address of the program: ";
    cin >> PC; // Set the starting address
    initialPC = PC;
    for (int i = 0; i < 32; i++) {
        reg[i] = 0; // Initialize registers to 0
    }
}

// Finding the value at memory address 'first'
int memFind(int first) {
    for (int i = 0; i < memory.size(); i++) {
        if (memory[i].first == first)
            return memory[i].second;
    }
}

// Writing the 'second' value to memory at address 'first'
void memWrite(int first, int second, bool isHalfword = false) {
    int index = (first - initialPC) / 4;

    // Ensure 'memory' has enough space up to 'index'
    for (int i = memory.size(); i <= index; i++) {
        int address = initialPC + (i * 4);  // Calculate each address incrementally
        memory.push_back({ address, 0 });     // Initialize with default {address, 0}
    }

    // Handle halfword storage if specified
    if (isHalfword) {
        // Extract the lower 16 bits of 'second' for halfword write
        int halfwordValue = second & 0xFFFF;

        // Combine the halfword with the existing word in memory (preserving upper 16 bits)
        memory[index].second = (memory[index].second & 0xFFFF0000) | halfwordValue;
    }
    else {
        // For a full word write, replace the entire word
        memory[index] = { first, second };
    }
}

string decimal_to_binary(int decimal) {

    string binary = "";

    if (decimal == 0) {
        return "0";
    }

    else {
        while (decimal > 0) {
            binary = to_string(decimal % 2) + binary;
            decimal /= 2;
        }
    }
    return binary;
}


string decimal_to_hexadecimal(int decimal) {

    string hex_characters = "0123456789ABCDEF";
    string hexadecimal = "";

    if (decimal == 0) {
        return "0";
    }

    else {
        while (decimal > 0) {
            int remainder = decimal % 16;
            hexadecimal = hex_characters[remainder] + hexadecimal;
            decimal /= 16;
        }
    }
    return hexadecimal;
}

// Formatted simulation output
void simDisplay() {
    // Output PC
    std::cout << "Program Counter: " << PC << "\n\n";

    if (reg[0] != 0) {
        cout << "WARNING: Attempt to manipulate zero register -- rejected." << endl;
        reg[0] = 0;
    }

    // Output RF
    std::cout << "Register File:" << std::endl;
    for (int i = 0; i < 32; ++i) {
        std::cout << "  Register [" << i << "]: " << std::setw(10) << reg[i] << std::setw(10) << " " << decimal_to_binary(reg[i]) << std::setw(10) << " " << decimal_to_hexadecimal(reg[i]) << std::endl;
    }
    std::cout << std::endl;

    // Output Memory
    std::cout << "Memory:" << std::endl;
    for (size_t i = 0; i < memory.size(); ++i) {
        std::cout << "  Location [" << i << "]: "
            << std::setw(4) << memory[i].first
            << " -> " << std::setw(10) << memory[i].second << std::setw(10) << " " << decimal_to_binary(memory[i].second) << std::setw(10) << " " << decimal_to_hexadecimal(memory[i].second) << std::endl;
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
        pair<int, int> x (PC + increment, 0);
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
        string rs1 = "";
        string rs2 = "";

        // Extract the action (instruction) part
        for (int j = 0; j < instruction[i].size(); j++) {
            if (instruction[i][j] == ' ') {
                break;
            }
            action += instruction[i][j];
        }

        cout << action << endl;
        if (action == "ECALL"|| action == "EBREAK" || action == "PAUSE" || action == "FENCE" || action == "FENCE.TSO")
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
            reg[rdIndex] = immediateValue * 4096; // Store the immediate value multiplied by 4096 // <<12 

            cout << "LUI: Loaded " << (immediateValue * 4096) << " into register " << rd << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            cout << rdIndex << endl;
            cout << reg[rdIndex] << endl;
            i++;
        }
        else if (action == "AUIPC")
        {
            PC = PC + 4;
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
            //PC = 356 
            int rdIndex, rs1Index;
            int immediateValue;
            cout << "===== pc ===== " << PC << endl;
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
            //2
            // reg[2]+0
            // 312 
            PC = reg[rs1Index] + immediateValue;
            //cout << "DD" << reg[rs1Index] + immediateValue << endl;
            cout << "PC: " << PC << endl;

            cout << "JALR: Jumping to address " << PC << " from register " << "x" << rs1Index << endl;
            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;
            //exit(0);
            // Update loop index based on new PC value

            // PC and i = relationship 
            //JALR x0,x2,0
            // initial = the first address of the memory
            // initialPC = 300
            //x2 = 312
            // instruction[312]
            //0-14
            // i = (312 - 300)/4 = 3
            // instruction[3] = LUI x4,77

            
            i = (PC - initialPC) / 4 ;
        }
        else if (action == "BEQ")
        {
            int rs1Index, rs2Index;
            int immediateValue;

             //Parse `rs1`, `rs2`, and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rs1, rs2, imm;

                 //Extract `rs1` (between the first space and first comma)
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index

                 //Extract `rs2` (between first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs2 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1);
                rs2Index = stoi(rs2.substr(1));  // Convert to index

                 //Extract `imm` (after the last comma)
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
                immediateValue = stoi(imm);  // Convert to integer
            }

            cout << "BEQ: Comparing registers x" << rs1Index << " and x" << rs2Index << endl;
            cout << "Values: " << reg[rs1Index] << " and " << reg[rs2Index] << endl;

             //Check if registers are equal
            if (reg[rs1Index] == reg[rs2Index]) {
                 //If equal, update the PC to the branch target address
                PC = PC + immediateValue;
                cout << "Branch taken. New PC: " << PC << endl;

                 //Update loop index based on new PC value
                i = (PC - initialPC) / 4;
            }
            else 
            {
                cout << "Branch not taken. Continuing to next instruction." << endl;
                PC = PC + 4;
                i++;
            }
         }

        else if (action == "BNE")
        {
            int rs1Index, rs2Index;
            int immediateValue;

            //Parse `rs1`, `rs2`, and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rs1, rs2, imm;

                //Extract `rs1` (between the first space and first comma)
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index

                //Extract `rs2` (between first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs2 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1);
                rs2Index = stoi(rs2.substr(1));  // Convert to index

                //Extract `imm` (after the last comma)
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
                immediateValue = stoi(imm);  // Convert to integer
            }

            cout << "BNE: Comparing registers x" << rs1Index << " and x" << rs2Index << endl;
            cout << "Values: " << reg[rs1Index] << " and " << reg[rs2Index] << endl;

            //Check if registers are equal
            if (reg[rs1Index] != reg[rs2Index]) {
                //If equal, update the PC to the branch target address
                PC = PC + immediateValue;
                cout << "BNE Branch taken. New PC: " << PC << endl;

                //Update loop index based on new PC value
                i = (PC - initialPC) / 4;
            }
            else
            {
                cout << "Branch not taken. Continuing to next instruction." << endl;
                PC = PC + 4;
                i++;
            }
        }
        // Load half-word unsigned
        else if (action == "LHU") {
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

            // Convert rd to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Convert offset (string) to an integer directly as decimal
            offset = stoi(imm); // Treat the input as a decimal value

            // Getting the value desired in memory at the memory address rs1 + offset
            int loadValue = memFind(reg[rs1Index] + offset);

            // Get LS 16 bits from the load value
            int loadHW = loadValue & 0xFFFF;

            // Loading the value into memory address at rd
            memWrite(reg[rdIndex], loadHW);
            cout << "Value " << loadHW << " written in memeory address " << reg[rdIndex] << endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
            }

        else if (action == "SB") {
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

                // Convert rs1 to register index (assuming rs1 is in the form "x0", "x1", etc.)
                rs1Index = stoi(rs1.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

                // Convert offset (string) to an integer directly as decimal
                offset = stoi(imm); // Treat the input as a decimal value

                // Getting the LS byte from register value
                int byte = reg[rs1Index] & 0xFF;

                // Storing the value into memory address at rd
                memWrite(reg[rdIndex + offset], byte);
                cout << "Value " << byte << " stored at memory address " << reg[rdIndex + offset] << endl;

                // Increment program counter & i to point to next instruction
                i++;
                PC += 4;
                }

        else if (action == "SH") {
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

                    // Convert rs1 to register index (assuming rs1 is in the form "x0", "x1", etc.)
                    rs1Index = stoi(rs1.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

                    // Convert offset (string) to an integer directly as decimal
                    offset = stoi(imm); // Treat the input as a decimal value

                    // Getting the LS halfword from register value
                    int hw = reg[rs1Index] & 0xFFFF;
                    cout << "hw:" << hw << endl;

                    // Storing the value into memory address at rd
                    memWrite(reg[rdIndex + offset], hw, true);
                    cout << "Value " << hw << " stored at memory address " << reg[rdIndex + offset] << endl;

                    // Increment program counter & i to point to next instruction
                    i++;
                    PC += 4;
                    }

        else if (action == "SW") {
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

                        // Convert rs1 to register index (assuming rs1 is in the form "x0", "x1", etc.)
                        rs1Index = stoi(rs1.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

                        // Convert offset (string) to an integer directly as decimal
                        offset = stoi(imm); // Treat the input as a decimal value

                        // Storing the value into memory address at rd
                        memWrite(reg[rdIndex] + offset, reg[rs1Index]);
                        cout << "Value " << reg[rs1Index] << " stored at memory address " << reg[rdIndex + offset] << endl;

                        // Increment program counter & i to point to next instruction
                        i++;
                        PC += 4;
                        }
        else if (action == "ADDI") 
        {
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find rs1 by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract imm after the last comma
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


            // Convert imm (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] << endl;

            // Perform the calculation
            reg[rdIndex] = reg[rs1Index] + immediateValue;
            cout << "Value " << reg[rdIndex] << " written in register " << rdIndex << endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
            }

        else if (action == "SLTI") {
            int rdIndex, rs1Index;
            int immediateValue;

            // Parse rd, rs1, and imm from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Find rs1 by locating the second comma and extracting the register
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                // Extract imm after the last comma
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

            // Convert imm (string) to an integer directly as decimal
            immediateValue = stoi(imm); // Treat the input as a decimal value
            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] << endl;

            // Perform the calculation
            if (reg[rs1Index] < immediateValue)
                reg[rdIndex] = 1;
            else
                reg[rdIndex] = 0;

            cout << "Register " << rdIndex << " set to " << reg[rdIndex] << endl;

            // Increment program counter & i to point to next instruction
            i++;
            PC += 4;
            }

        else if (action == "SLTIU") {
                int rdIndex, rs1Index;
                int immediateValue;

                // Parse rd, rs1, and imm from the instruction
                size_t pos = instruction[i].find(" ");
                if (pos != string::npos) {
                    rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                    // Find rs1 by locating the second comma and extracting the register
                    size_t pos2 = instruction[i].find(",", pos + 1);
                    rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                    // Extract imm after the last comma
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

                // Convert imm (string) to an integer directly as decimal
                immediateValue = stoi(imm); // Treat the input as a decimal value
                cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] << endl;

                // Perform the calculation
                if (abs(reg[rs1Index]) < abs(immediateValue))
                    reg[rdIndex] = 1;
                else
                    reg[rdIndex] = 0;

                cout << "Register " << rdIndex << " set to " << reg[rdIndex] << endl;

                // Increment program counter & i to point to next instruction
                i++;
                PC += 4;
                }

        else if (action == "XORI") {
                    int rdIndex, rs1Index;
                    int immediateValue;

                    // Parse rd, rs1, and imm from the instruction
                    size_t pos = instruction[i].find(" ");
                    if (pos != string::npos) {
                        rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                        // Find rs1 by locating the second comma and extracting the register
                        size_t pos2 = instruction[i].find(",", pos + 1);
                        rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                        // Extract imm after the last comma
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


                    // Convert imm (string) to an integer directly as decimal
                    immediateValue = stoi(imm); // Treat the input as a decimal value
                    cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] << endl;

                    // Perform the calculation
                    reg[rdIndex] = reg[rs1Index] ^ immediateValue;
                    cout << "Value " << reg[rdIndex] << " written in register " << rdIndex << endl;

                    // Increment program counter & i to point to next instruction
                    i++;
                    PC += 4;
                    }

        else if (action == "ORI") {
                        int rdIndex, rs1Index;
                        int immediateValue;

                        // Parse rd, rs1, and imm from the instruction
                        size_t pos = instruction[i].find(" ");
                        if (pos != string::npos) {
                            rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                            // Find rs1 by locating the second comma and extracting the register
                            size_t pos2 = instruction[i].find(",", pos + 1);
                            rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                            // Extract imm after the last comma
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


                        // Convert imm (string) to an integer directly as decimal
                        immediateValue = stoi(imm); // Treat the input as a decimal value
                        cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] << endl;

                        // Perform the calculation
                        reg[rdIndex] = reg[rs1Index] | immediateValue;
                        cout << "Value " << reg[rdIndex] << " written in register " << rdIndex << endl;

                        // Increment program counter & i to point to next instruction
                        i++;
                        PC += 4;
                        }

        else if (action == "ANDI") {
                            int rdIndex, rs1Index;
                            int immediateValue;

                            // Parse rd, rs1, and imm from the instruction
                            size_t pos = instruction[i].find(" ");
                            if (pos != string::npos) {
                                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                                // Find rs1 by locating the second comma and extracting the register
                                size_t pos2 = instruction[i].find(",", pos + 1);
                                rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                                // Extract imm after the last comma
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


                            // Convert imm (string) to an integer directly as decimal
                            immediateValue = stoi(imm); // Treat the input as a decimal value
                            cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] << endl;

                            // Perform the calculation
                            reg[rdIndex] = reg[rs1Index] & immediateValue;
                            cout << "Value " << reg[rdIndex] << " written in register " << rdIndex << endl;

                            // Increment program counter & i to point to next instruction
                            i++;
                            PC += 4;
                            }

        else if (action == "SLLI") {
                                int rdIndex, rs1Index;
                                int immediateValue;

                                // Parse rd, rs1, and imm from the instruction
                                size_t pos = instruction[i].find(" ");
                                if (pos != string::npos) {
                                    rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                                    // Find rs1 by locating the second comma and extracting the register
                                    size_t pos2 = instruction[i].find(",", pos + 1);
                                    rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                                    // Extract imm after the last comma
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


                                // Convert imm (string) to an integer directly as decimal
                                immediateValue = stoi(imm); // Treat the input as a decimal value
                                cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] << endl;

                                // Perform the calculation
                                reg[rdIndex] = reg[rs1Index] * (pow(2, immediateValue));
                                cout << "Value " << reg[rdIndex] << " written in register " << rdIndex << endl;

                                // Increment program counter & i to point to next instruction
                                i++;
                                PC += 4;
                                }

        else if (action == "SRLI") {
                                    int rdIndex, rs1Index;
                                    int immediateValue;

                                    // Parse rd, rs1, and imm from the instruction
                                    size_t pos = instruction[i].find(" ");
                                    if (pos != string::npos) {
                                        rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                                        // Find rs1 by locating the second comma and extracting the register
                                        size_t pos2 = instruction[i].find(",", pos + 1);
                                        rs1 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1).substr(1);


                                        // Extract imm after the last comma
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

                                    // Convert imm (string) to an integer directly as decimal
                                    immediateValue = stoi(imm); // Treat the input as a decimal value
                                    cout << immediateValue << " " << rs1Index << " " << reg[rs1Index] << endl;

                                    // Perform the calculation
                                    reg[rdIndex] = reg[rs1Index] / (pow(2, immediateValue));
                                    cout << "Value " << reg[rdIndex] << " written in register " << rdIndex << endl;

                                    // Increment program counter & i to point to next instruction
                                    i++;
                                    PC += 4;
                                    }
        else if (action == "BLT")
        {
            int rs1Index, rs2Index;
            int immediateValue;

            //Parse `rs1`, `rs2`, and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rs1, rs2, imm;

                //Extract `rs1` (between the first space and first comma)
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index

                //Extract `rs2` (between first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs2 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1);
                rs2Index = stoi(rs2.substr(1));  // Convert to index

                //Extract `imm` (after the last comma)
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
                immediateValue = stoi(imm);  // Convert to integer
            }

            cout << "BLE: Comparing registers x" << rs1Index << " and x" << rs2Index << endl;
            cout << "Values: " << reg[rs1Index] << " and " << reg[rs2Index] << endl;

            //Check if registers are equal
            if (reg[rs1Index] < reg[rs2Index]) {
                //If equal, update the PC to the branch target address
                PC = PC + immediateValue;
                cout << "BLE Branch taken. New PC: " << PC << endl;

                //Update loop index based on new PC value
                i = (PC - initialPC) / 4;
            }
            else
            {
                cout << "Branch not taken. Continuing to next instruction." << endl;
                PC = PC + 4;
                i++;
            }
        }
        else if (action == "BGE")
        {
            int rs1Index, rs2Index;
            int immediateValue;

            //Parse `rs1`, `rs2`, and `imm` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rs1, rs2, imm;

                //Extract `rs1` (between the first space and first comma)
                rs1 = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index

                //Extract `rs2` (between first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                rs2 = instruction[i].substr(pos2 + 1, instruction[i].find(",", pos2 + 1) - pos2 - 1);
                rs2Index = stoi(rs2.substr(1));  // Convert to index

                //Extract `imm` (after the last comma)
                imm = instruction[i].substr(instruction[i].rfind(",") + 1);
                immediateValue = stoi(imm);  // Convert to integer
            }

            cout << "BGE: Comparing registers x" << rs1Index << " and x" << rs2Index << endl;
            cout << "Values: " << reg[rs1Index] << " and " << reg[rs2Index] << endl;

            //Check if registers are equal
            if (reg[rs1Index] > reg[rs2Index]) {
                //If equal, update the PC to the branch target address
                PC = PC + immediateValue;
                cout << "BGE Branch taken. New PC: " << PC << endl;

                //Update loop index based on new PC value
                i = (PC - initialPC) / 4;
            }
            else
            {
                cout << "Branch not taken. Continuing to next instruction." << endl;
                PC = PC + 4;
                i++;
            }
        }
        else if (action == "LB")
        {
            int rdIndex, rs1Index, offset;

            // Parse `rd`, `rs1`, and `offset` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rd, rs1, offsetStr;

                // Extract `rd` (between the first space and first comma)
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rdIndex = stoi(rd.substr(1));  // Convert to index

                // Extract `offset` (between the first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                offsetStr = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2 + 1) - pos2 - 1);
                offset = stoi(offsetStr);  // Convert to integer

                // Extract `rs1` (between parentheses after `offset`)
                size_t pos3 = instruction[i].find("(", pos2 + 1);
                rs1 = instruction[i].substr(pos3 + 1, instruction[i].find(")", pos3) - pos3 - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index
            }

            // Calculate the memory address (rs1 + offset)
            int address = reg[rs1Index] + offset;

            // Fetch the byte from memory at the calculated address
            cout << "ADDRESS" << address << endl;
            int8_t byte = memory[(address - initialPC) / 4].second; // Assuming memory is a byte-addressable array/vector

            // Sign-extend the byte to 32 bits
            int32_t sign_extended_byte = static_cast<int32_t>(byte);

            // Store the result in the target register `rd`
            reg[rdIndex] = sign_extended_byte;

            cout << "LB: Loaded byte from address " << address << " into register x" << rdIndex << endl;
            cout << "Value in x" << rdIndex << ": " << reg[rdIndex] << endl;

            // Increment the program counter to the next instruction
            PC += 4;
            i++;
            }
        else if (action == "LI") 
        {
            int rdIndex;           // Register to store the immediate value
            int immediate;         // The immediate value to load

            // Parse rd and immediate from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != std::string::npos) {
                // Parsing rd by locating the first space and comma characters
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);

                // Parsing immediate value (everything after the comma)
                size_t pos2 = instruction[i].find(",");
                std::string immStr = instruction[i].substr(pos2 + 1);

                // Convert immediate to an integer directly (assuming decimal)
                immediate = std::stoi(immStr);
            }

            // Convert rd to register index (assuming rd is in the form "x0", "x1", etc.)
            rdIndex = std::stoi(rd.substr(1)); // Convert "x1" to 1, "x2" to 2, etc.

            // Write the immediate value to the register array at the rdIndex
            reg[rdIndex] = immediate;
            std::cout << "Loaded immediate " << immediate << " into register x" << rdIndex << std::endl;

            // Increment program counter and instruction index to point to the next instruction
            i++;
            PC += 4;
            }
        else if (action == "LW")
        {
            int rdIndex, rs1Index, offset;
            // Parse `rd`, `rs1`, and `offset` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rd, rs1, offsetStr;

                // Extract `rd` (between the first space and first comma)
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rdIndex = stoi(rd.substr(1));  // Convert to index

                // Extract `offset` (between the first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                offsetStr = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2 + 1) - pos2 - 1);
                offset = stoi(offsetStr);  // Convert to integer

                // Extract `rs1` (between parentheses after `offset`)
                size_t pos3 = instruction[i].find("(", pos2 + 1);
                rs1 = instruction[i].substr(pos3 + 1, instruction[i].find(")", pos3) - pos3 - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index
            }

            // Calculate the memory address (rs1 + offset)
            int address = reg[rs1Index] + offset;

            // Fetch the byte from memory at the calculated address
            int32_t byte = memory[(address - initialPC) / 4].second; // Assuming memory is a byte-addressable array/vector

            reg[rdIndex] = byte;

            cout << "LB: Loaded byte from address " << address << " into register x" << rdIndex << endl;
            cout << "Value in x" << rdIndex << ": " << reg[rdIndex] << endl;

            // Increment the program counter to the next instruction
            PC += 4;
            i++;
        }
        else if (action == "LH")
        {
            int rdIndex, rs1Index, offset;

            // Parse `rd`, `rs1`, and `offset` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rd, rs1, offsetStr;

                // Extract `rd` (between the first space and first comma)
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rdIndex = stoi(rd.substr(1));  // Convert to index

                // Extract `offset` (between the first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                offsetStr = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2 + 1) - pos2 - 1);
                offset = stoi(offsetStr);  // Convert to integer

                // Extract `rs1` (between parentheses after `offset`)
                size_t pos3 = instruction[i].find("(", pos2 + 1);
                rs1 = instruction[i].substr(pos3 + 1, instruction[i].find(")", pos3) - pos3 - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index
            }
            cout << rs1Index << " " << offset << " " << endl;
            cout << reg[rs1Index] << endl;
            int address = 0;
            
            // Calculate the memory address (rs1 + offset)
            address = reg[rs1Index] + offset;

            cout << address << endl;
           
            //cout << "==========" << (address - initialPC) / 4 << endl;

            // Fetch the byte from memory at the calculated address
            int16_t byte = memory[(address - initialPC)/4].second; // Assuming memory is a byte-addressable array/vector

            // Sign-extend the byte to 32 bits
            int32_t sign_extended_byte = static_cast<int32_t>(byte);

            // Store the result in the target register `rd`
            reg[rdIndex] = sign_extended_byte;

            cout << "LB: Loaded byte from address " << address << " into register x" << rdIndex << endl;
            cout << "Value in x" << rdIndex << ": " << reg[rdIndex] << endl;

            // Increment the program counter to the next instruction
            PC += 4;
            i++;
        }
        else if (action == "LBU")
        {
            int rdIndex, rs1Index, offset;

            // Parse `rd`, `rs1`, and `offset` from the instruction
            size_t pos = instruction[i].find(" ");
            if (pos != string::npos) {
                string rd, rs1, offsetStr;

                // Extract `rd` (between the first space and first comma)
                rd = instruction[i].substr(pos + 1, instruction[i].find(",", pos) - pos - 1);
                rdIndex = stoi(rd.substr(1));  // Convert to index

                // Extract `offset` (between the first and second commas)
                size_t pos2 = instruction[i].find(",", pos + 1);
                offsetStr = instruction[i].substr(pos2 + 1, instruction[i].find("(", pos2 + 1) - pos2 - 1);
                offset = stoi(offsetStr);  // Convert to integer

                // Extract `rs1` (between parentheses after `offset`)
                size_t pos3 = instruction[i].find("(", pos2 + 1);
                rs1 = instruction[i].substr(pos3 + 1, instruction[i].find(")", pos3) - pos3 - 1);
                rs1Index = stoi(rs1.substr(1));  // Convert to index
            }

            // Calculate the memory address (rs1 + offset)
            int address = reg[rs1Index] + offset;

            // Fetch the byte from memory at the calculated address
            uint8_t byte = memory[((address - initialPC) / 4) / 4].second; // Assuming memory is a byte-addressable array/vector

            // Sign-extend the byte to 32 bits
            uint32_t sign_extended_byte = static_cast<int32_t>(byte);

            // Store the result in the target register `rd`
            reg[rdIndex] = sign_extended_byte;

            cout << "LB: Loaded byte from address " << address << " into register x" << rdIndex << endl;
            cout << "Value in x" << rdIndex << ": " << reg[rdIndex] << endl;

            // Increment the program counter to the next instruction
            PC += 4;
            i++;
            }
        else if (action == "SUB") {
            int rdIndex, rs1Index, rs2Index;
            string rd, rs1, rs2;
            // Parsing of rd, rs1, rs2 from instruction
            size_t pos = instruction[i].find(" "); // First space after the instruction name
            size_t pos2 = instruction[i].find(",", pos + 1); // First comma
            size_t pos3 = instruction[i].find(",", pos2 + 1); // Second comma


            if (pos != string::npos) {
                // Extracting rd
                rd = instruction[i].substr(pos + 1, pos2 - pos - 1);
                rdIndex = stoi(rd.substr(1)); // converts register number into its index (e.g. x1 -> 1)

                // Extracting rs1 
                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1);
                rs1Index = stoi(rs1.substr(1));

                // Extracting rs2
                rs2 = instruction[i].substr(pos3 + 1);
                rs2Index = stoi(rs2.substr(1));
            }
            reg[rdIndex] = reg[rs1Index] - reg[rs2Index]; // The subtraction process itself

            cout << "SUB: Subtracting x" << rs2Index << " from x" << rs1Index << " and storing result in x" << rdIndex << endl;
            cout << "Register x" << rdIndex << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

            PC += 4; // updating program counter
            i++; // moving to the next instruction
            }

        else if (action == "SLL") {
                int rdIndex, rs1Index, rs2Index;
                string rd, rs1, rs2;

                // Parsing rd, rs1, and rs2 from instruction
                size_t pos = instruction[i].find(" ");
                size_t pos2 = instruction[i].find(",", pos + 1);
                size_t pos3 = instruction[i].find(",", pos2 + 1);

                if (pos != string::npos) {
                    rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                    rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                    rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                }
                // converts register number into its index
                rdIndex = stoi(rd.substr(1));
                rs1Index = stoi(rs1.substr(1));
                rs2Index = stoi(rs2.substr(1));

                // Shift the value in rs1 by the amount in rs2
                reg[rdIndex] = reg[rs1Index] << (reg[rs2Index]);

                cout << "SLL: Shifted value in register " << rs1 << " left by " << (reg[rs2Index])
                    << " positions. Stored result in register " << rd << endl;
                cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                PC += 4;
                i++;
                }


        else if (action == "SLT") {
                    int rdIndex, rs1Index, rs2Index;
                    string rd, rs1, rs2;

                    // Parse rd, rs1, and rs2 from the instruction
                    size_t pos = instruction[i].find(" ");
                    size_t pos2 = instruction[i].find(",", pos + 1);
                    size_t pos3 = instruction[i].find(",", pos2 + 1);

                    if (pos != string::npos) {

                        rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                        rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                        rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                    }

                    // converts register number into its index
                    rdIndex = stoi(rd.substr(1));
                    rs1Index = stoi(rs1.substr(1));
                    rs2Index = stoi(rs2.substr(1));

                    // Perform the SLT operation
                    if (reg[rs1Index] < reg[rs2Index]) {
                        reg[rdIndex] = 1; // if rs1 less rs2 than, set value to 1
                    }
                    else {
                        reg[rdIndex] = 0; // if not, set to 0
                    }

                    cout << "SLT: Compared registers " << rs1 << " and " << rs2 << ". ";
                    cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;


                    PC += 4;
                    i++;
                    }

        else if (action == "SLTU") {
                        int rdIndex, rs1Index, rs2Index;
                        string rd, rs1, rs2;

                        // Parse rd, rs1, and rs2 from the instruction
                        size_t pos = instruction[i].find(" ");
                        size_t pos2 = instruction[i].find(",", pos + 1);
                        size_t pos3 = instruction[i].find(",", pos2 + 1);

                        if (pos != string::npos) {

                            rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                            rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                            rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                        }

                        // converts register number into its index
                        rdIndex = stoi(rd.substr(1));
                        rs1Index = stoi(rs1.substr(1));
                        rs2Index = stoi(rs2.substr(1));

                        // Perform the SLTU operation (same as SLT but unsigned)
                        if (static_cast<unsigned>(reg[rs1Index]) < static_cast<unsigned>(reg[rs2Index])) {
                            reg[rdIndex] = 1;
                        }
                        else {
                            reg[rdIndex] = 0;
                        }


                        cout << "SLTU: Compared registers " << rs1 << " and " << rs2 << ". ";
                        cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                        PC += 4;
                        i++;
                        }

        else if (action == "SRL") {
                            int rdIndex, rs1Index, rs2Index;
                            string rd, rs1, rs2;

                            // Parse rd, rs1, and rs2 from the instruction
                            size_t pos = instruction[i].find(" ");
                            size_t pos2 = instruction[i].find(",", pos + 1);
                            size_t pos3 = instruction[i].find(",", pos2 + 1);

                            if (pos != string::npos) {
                                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1);  // Extract rs1
                                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                            }

                            // Convert register number into its index
                            rdIndex = stoi(rd.substr(1));
                            rs1Index = stoi(rs1.substr(1));
                            rs2Index = stoi(rs2.substr(1));

                            // Perform the shift right logical operation
                            reg[rdIndex] = static_cast<unsigned>(reg[rs1Index]) >> (reg[rs2Index]);


                            cout << "SRL: Shifted value in register " << rs1 << " right by " << (reg[rs2Index])
                                << " positions. Stored result in register " << rd << endl;
                            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                            PC += 4;
                            i++;
                            }

        else if (action == "XOR") {

                                int rdIndex, rs1Index, rs2Index;
                                string rd, rs1, rs2;

                                // Parse rd, rs1, and rs2 from the instruction
                                size_t pos = instruction[i].find(" ");
                                size_t pos2 = instruction[i].find(",", pos + 1);
                                size_t pos3 = instruction[i].find(",", pos2 + 1);

                                if (pos != string::npos) {
                                    rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                    rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                                    rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                                }

                                // Convert register number into its index
                                rdIndex = stoi(rd.substr(1));
                                rs1Index = stoi(rs1.substr(1));
                                rs2Index = stoi(rs2.substr(1));

                                // Perform the bitwise XOR operation
                                reg[rdIndex] = reg[rs1Index] ^ reg[rs2Index];

                                cout << "XOR: XORed registers " << rs1 << " and " << rs2
                                    << ". Stored result in register " << rd << endl;
                                cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                                PC += 4;
                                i++;
                                }

        else if (action == "OR") {
                                    int rdIndex, rs1Index, rs2Index;
                                    string rd, rs1, rs2;

                                    // Parse rd, rs1, and rs2 from the instruction
                                    size_t pos = instruction[i].find(" ");
                                    size_t pos2 = instruction[i].find(",", pos + 1);
                                    size_t pos3 = instruction[i].find(",", pos2 + 1);

                                    if (pos != string::npos) {
                                        rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                        rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                                        rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                                    }

                                    // Convert register numbers to indices
                                    rdIndex = stoi(rd.substr(1));
                                    rs1Index = stoi(rs1.substr(1));
                                    rs2Index = stoi(rs2.substr(1));

                                    // ORing rs1 and rs2 (bitwise OR)
                                    reg[rdIndex] = reg[rs1Index] | reg[rs2Index];


                                    cout << "OR: Performed bitwise OR between registers " << rs1 << " and " << rs2;
                                    cout << ". Stored result in register " << rd << endl;
                                    cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                                    PC += 4;
                                    i++;
                                    }

        else if (action == "AND") {
                                        int rdIndex, rs1Index, rs2Index;
                                        string rd, rs1, rs2;

                                        // Parse rd, rs1, and rs2 from the instruction
                                        size_t pos = instruction[i].find(" ");
                                        size_t pos2 = instruction[i].find(",", pos + 1);
                                        size_t pos3 = instruction[i].find(",", pos2 + 1);

                                        if (pos != string::npos) {
                                            rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                            rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                                            rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                                        }

                                        // Convert register numbers to indices
                                        rdIndex = stoi(rd.substr(1));
                                        rs1Index = stoi(rs1.substr(1));
                                        rs2Index = stoi(rs2.substr(1));

                                        // AND: rs1 & rs2 (bitwise AND)
                                        reg[rdIndex] = reg[rs1Index] & reg[rs2Index];

                                        cout << "AND: Performed bitwise AND between registers " << rs1 << " and " << rs2;
                                        cout << ". Stored result in register " << rd << endl;
                                        cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                                        PC += 4;
                                        i++;
                                        }

        else if (action == "ADD") {
                                            int rdIndex, rs1Index, rs2Index;
                                            string rd, rs1, rs2;

                                            // Parse rd, rs1, and rs2 from the instruction
                                            size_t pos = instruction[i].find(" ");
                                            size_t pos2 = instruction[i].find(",", pos + 1);
                                            size_t pos3 = instruction[i].find(",", pos2 + 1);

                                            if (pos != string::npos) {
                                                rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                                rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1);  // Extract rs1
                                                rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                                            }

                                            // Convert register names to indices
                                            rdIndex = stoi(rd.substr(1));
                                            rs1Index = stoi(rs1.substr(1));
                                            rs2Index = stoi(rs2.substr(1));

                                            // Perform the addition
                                            reg[rdIndex] = reg[rs1Index] + reg[rs2Index];

                                            cout << "ADD: Added registers " << rs1 << " and " << rs2;
                                            cout << ". Stored result in register " << rd << endl;
                                            cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                                            PC += 4; // update program counter
                                            i++;
                                            }


        else if (action == "SRA") {
                                                int rdIndex, rs1Index, rs2Index;
                                                string rd, rs1, rs2;

                                                // Parse rd, rs1, and rs2 from the instruction
                                                size_t pos = instruction[i].find(" ");
                                                size_t pos2 = instruction[i].find(",", pos + 1);
                                                size_t pos3 = instruction[i].find(",", pos2 + 1);

                                                if (pos != string::npos) {
                                                    rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                                    rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                                                    rs2 = instruction[i].substr(pos3 + 1); // Extract rs2
                                                }

                                                // Convert register number into its index
                                                rdIndex = stoi(rd.substr(1));
                                                rs1Index = stoi(rs1.substr(1));
                                                rs2Index = stoi(rs2.substr(1));

                                                // SRA (preserves the sign value)
                                                if (reg[rs1Index] < 0) {

                                                    // to manually extend the sign bit for negative numbers
                                                    reg[rdIndex] = (reg[rs1Index] >> reg[rs2Index]) | (0xFFFFFFFF << (32 - reg[rs2Index]));
                                                    // that way, if the value is negative, it does the regular shift right operation
                                                    // then, it masks the bits placed after the shifting by using a bitwise OR operation with a 32-bit value of 1

                                                }
                                                else {

                                                    // a regular right shift for positive numbers
                                                    reg[rdIndex] = reg[rs1Index] >> reg[rs2Index];
                                                }



                                                cout << "SRA: Shifted value in register " << rs1 << " right (arithmetic) by "
                                                    << (reg[rs2Index]) << " positions. Stored result in register " << rd << endl;
                                                cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;


                                                PC += 4;
                                                i++;
                                                }

        else if (action == "SRAI") {
                                                    int rdIndex, rs1Index;
                                                    string rd, rs1;
                                                    int immediate;

                                                    // Parse rd, rs1, and immediate from the instruction
                                                    size_t pos = instruction[i].find(" ");
                                                    size_t pos2 = instruction[i].find(",", pos + 1);
                                                    size_t pos3 = instruction[i].find(",", pos2 + 1);

                                                    if (pos != string::npos) {
                                                        rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                                        rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                                                        immediate = stoi(instruction[i].substr(pos3 + 1)); // Extract immediate value and convert to integer
                                                    }

                                                    // Convert register names to indices
                                                    rdIndex = stoi(rd.substr(1));
                                                    rs1Index = stoi(rs1.substr(1));

                                                    // Performs arithmetic shift by immediate value (preserves the sign bit)
                                                    reg[rdIndex] = reg[rs1Index] >> (immediate);

                                                    cout << "SRAI: Shifted register " << rs1 << " right by " << (immediate)
                                                        << " positions. Stored result in register " << rd << endl;
                                                    cout << "Register " << rd << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                                                    PC += 4;
                                                    i++;
                                                    }

        else if (action == "SRLI") {
                                                        int rdIndex, rs1Index;
                                                        int immediate;

                                                        // Parse rd, rs1, and immediate from the instruction
                                                        size_t pos = instruction[i].find(" ");
                                                        size_t pos2 = instruction[i].find(",", pos + 1);
                                                        size_t pos3 = instruction[i].find(",", pos2 + 1);

                                                        if (pos != string::npos) {
                                                            string rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                                            string rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                                                            string immediate_string = instruction[i].substr(pos3 + 1); // Extract immediate

                                                            // Convert register names to indices
                                                            rdIndex = stoi(rd.substr(1));
                                                            rs1Index = stoi(rs1.substr(1));
                                                            immediate = stoi(immediate_string);
                                                        }

                                                        // Perform the SRLI operation (logical right shift)
                                                        reg[rdIndex] = static_cast<unsigned>(reg[rs1Index]) >> (immediate);

                                                        cout << "SRLI: Shifted value in register x" << rs1Index << " right by " << immediate
                                                            << " positions. Stored result in register x" << rdIndex << endl;
                                                        cout << "Register x" << rdIndex << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;


                                                        PC += 4;
                                                        i++;
                                                        }

        else if (action == "SLLI") {
                                                            int rdIndex, rs1Index;
                                                            int immediate;

                                                            // Parse rd, rs1, and immediate from the instruction
                                                            size_t pos = instruction[i].find(" ");
                                                            size_t pos2 = instruction[i].find(",", pos + 1);
                                                            size_t pos3 = instruction[i].find(",", pos2 + 1);

                                                            if (pos != string::npos) {
                                                                string rd = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rd
                                                                string rs1 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs1
                                                                string immediate_string = instruction[i].substr(pos3 + 1); // Extract immediate

                                                                // Convert register names to indices
                                                                rdIndex = stoi(rd.substr(1));
                                                                rs1Index = stoi(rs1.substr(1));
                                                                immediate = stoi(immediate_string);
                                                            }

                                                            // Perform the SLLI operation (logical left shift)
                                                            reg[rdIndex] = reg[rs1Index] << (immediate);

                                                            cout << "SLLI: Shifted value in register x" << rs1Index << " left by " << immediate
                                                                << " positions. Stored result in register x" << rdIndex << endl;
                                                            cout << "Register x" << rdIndex << " now contains: " << reg[rdIndex] << " (Decimal)" << endl;

                                                            PC += 4;
                                                            i++;
                                                            }

        else if (action == "BLTU") {
                                                                int rs1Index, rs2Index, offset;

                                                                // Parse rd, rs1, and immediate from the instruction
                                                                size_t pos = instruction[i].find(" ");
                                                                size_t pos2 = instruction[i].find(",", pos + 1);
                                                                size_t pos3 = instruction[i].find(",", pos2 + 1);

                                                                if (pos != string::npos) {
                                                                    string rs1 = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rs1
                                                                    string rs2 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs2
                                                                    string offset_string = instruction[i].substr(pos3 + 1); // Extract offset

                                                                    // Convert register names to indices
                                                                    rs1Index = stoi(rs1.substr(1));
                                                                    rs2Index = stoi(rs2.substr(1));
                                                                    offset = stoi(offset_string);
                                                                }
                                                                if (static_cast<unsigned>(reg[rs1Index]) < static_cast<unsigned>(reg[rs2Index])) {
                                                                    PC += offset;
                                                                    i = (PC - initialPC) / 4; // move to the next instruction according to where the PC is pointing to
                                                                }
                                                                else {
                                                                    PC += 4;
                                                                    i++;
                                                                }
                                                                }

        else if (action == "BGEU") {
                                                                    int rs1Index, rs2Index, offset;

                                                                    // Parse rd, rs1, and immediate from the instruction
                                                                    size_t pos = instruction[i].find(" ");
                                                                    size_t pos2 = instruction[i].find(",", pos + 1);
                                                                    size_t pos3 = instruction[i].find(",", pos2 + 1);

                                                                    if (pos != string::npos) {
                                                                        string rs1 = instruction[i].substr(pos + 1, pos2 - pos - 1); // Extract rs1
                                                                        string rs2 = instruction[i].substr(pos2 + 1, pos3 - pos2 - 1); // Extract rs2
                                                                        string offset_string = instruction[i].substr(pos3 + 1); // Extract offset

                                                                        // Convert register names to indices
                                                                        rs1Index = stoi(rs1.substr(1));
                                                                        rs2Index = stoi(rs2.substr(1));
                                                                        offset = stoi(offset_string);
                                                                    }
                                                                    if (static_cast<unsigned>(reg[rs1Index]) >= static_cast<unsigned>(reg[rs2Index])) {
                                                                        PC += offset;
                                                                        i = (PC - initialPC) / 4; // move to the next instruction according to where the PC is pointing to
                                                                    }
                                                                    else {
                                                                        PC += 4;
                                                                        i++;
                                                                    }
                                                                    }
        


        // Add additional instructions here as needed
        else {
            cout << "Unknown instruction: " << action << endl;
            exit(0);
        }
        simDisplay();
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


