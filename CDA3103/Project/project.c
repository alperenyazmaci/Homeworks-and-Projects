#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    
    //Create a temporary integer Z to store the result
    unsigned Z;
    //switch statement to do the operation according to ALUControl
    switch(ALUControl){
        case 0b000:
            Z = A + B;
            break;
        case 0b001: 
            Z = A - B;
            break;
        case 0b010:   // Unsigned 
            if((int)A < (int)B){
                Z = 0b1;
            }else{
                Z = 0b0;
            }
            break;
        case 0b011: // Signed
            if(A < B){
                Z = 0b1;
            }
            else{
                Z = 0b0;
            }
            break;
        case 0b100: 
            Z = A & B;
            break;
        case 0b101: 
            Z = A | B;
            break;
        case 0b110: 
            Z = B << 16;
            break;
        case 0b111: 
            Z = ~A;
            break;
    }

    //put the value in Z (result) into the ALUresult
    *ALUresult = Z;

    //set Zero to 1 if Z equals 0, set Zero to 0 otherwise
    if(Z == 0){
        *Zero = 1;
    }else{
        *Zero = 0;
    }


}
/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    unsigned wholeMachineCode = Mem[PC >> 2]; //get the machine code (in base 10) and assign it to wholeMachineCode
    *instruction = wholeMachineCode; //put wholeMachineCode in the instruction

    //check if a halt occurs
    if(wholeMachineCode == 0){
        return 1;
    }else{
        return 0;
    }
}
/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned
*r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    unsigned rPartition		    = 0x1f; //5 bits total
	unsigned functopPartition	= 0x0000003f; //6 bits total
	unsigned offsetPartition	= 0x0000ffff; //16 bits total
	unsigned jsecPartition	    = 0x03ffffff; //26 bits total

    *op = instruction >> 26;
    *r1 = instruction >> 21 & 0x1F;
    *r2 = instruction >> 16 & 0x1F;
    *r3 = instruction >> 11 & 0x1F;
    *funct = instruction & 0x3F;
    *offset = instruction & 0xFFFF;
    *jsec = instruction & 0x3FFFFFF;
}
/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // Put the values for the different controls in the struct

    switch(op){

        //R-Type
        case 0b000000:    //0 0        
            controls->ALUOp = 0b111;
            controls->ALUSrc = 0b0;
            controls->Branch = 0b0;
            controls->Jump = 0b0;
            controls->MemRead = 0b0;
            controls->MemWrite = 0b0;
            controls->RegDst = 0b1;
            controls->RegWrite = 0b1;
            controls->MemtoReg = 0b0;
            break;
        // Jump 
        case 0b000010:    //0 2         
            controls->ALUOp = 0;        
            controls->ALUSrc = 0b0;
            controls->Branch =  0b0;
            controls->Jump = 0b1;
            controls->MemRead = 0b0;
            controls->MemWrite = 0b0;
            controls->RegDst = 0b0;
            controls->RegWrite = 0b0;
            controls->MemtoReg = 0b0;
            break;
        // Branch eq
        case 0b000100:    //0 4         
            controls->ALUOp = 0b001;        
            controls->ALUSrc = 0b0;
            controls->Branch =  0b1;
            controls->Jump = 0b0;
            controls->MemRead =0b0;
            controls->MemWrite = 0b0;
            controls->RegDst = 0b10;
            controls->RegWrite = 0b0;
            controls->MemtoReg = 0b10;
            break;
        
        //add immediate
        case 0b001000:    //1 0            
            controls->ALUOp = 0b000;
            controls->ALUSrc = 0b1;
            controls->Branch =  0b0;
            controls->Jump = 0b0;
            controls->MemRead =0b0;
            controls->MemWrite = 0b0;
            controls->RegDst = 0b0;
            controls->RegWrite = 0b1;
            controls->MemtoReg = 0b0;
            break;

        //set less than imm.               
        case 0b001010:    //1 2
            controls->ALUOp = 0b010;
            controls->ALUSrc = 0b1;         
            controls->Branch =  0b0;        
            controls->Jump = 0b0;   
            controls->MemRead =0b0;        
            controls->MemWrite = 0b0;
            controls->RegDst = 0b0;
            controls->RegWrite = 0b1;
            controls->MemtoReg = 0b0;
            break;

        //sltiu
        case 0b001011:    //1 3         
            controls->ALUOp = 0b011;
            controls->ALUSrc = 0b1;     
            controls->Branch =  0b0;
            controls->Jump = 0b0;
            controls->MemRead =0b0;
            controls->MemWrite = 0b0;
            controls->RegDst = 0b0;
            controls->RegWrite = 0b1;
            controls->MemtoReg = 0b0;
            break;

        //load upper imm                
        case 0b001111:    //1 7
            controls->ALUOp = 0b110;
            controls->ALUSrc = 0b1;
            controls->Branch = 0b0;
            controls->Jump = 0b0;
            controls->MemRead = 0b0;
            controls->MemWrite = 0b0;
            controls->RegDst = 0b0;
            controls->RegWrite = 0b1;
		    controls->MemtoReg = 0b0;
            break;

        //loadword                      
        case 0b100011:    //4 3
            controls->ALUOp = 0b000;
            controls->ALUSrc = 0b1;    
            controls->Branch =  0b0;
            controls->Jump = 0b0;
            controls->MemRead =0b1;
            controls->MemWrite = 0b0;   
            controls->RegDst = 0b0;
            controls->RegWrite = 0b1;
            controls->MemtoReg = 0b1;
            break;

        //store word                    
        case 0b101011:    //5 3
            controls->ALUOp = 0b000;
            controls->ALUSrc = 0b1;     
            controls->Branch =  0b0;
            controls->Jump = 0b0;
            controls->MemRead =0b0;        
            controls->MemWrite = 0b1;
            controls->RegDst = 0b0;       
            controls->RegWrite = 0b0;
            controls->MemtoReg = 0b10;
            break;

        default:
            return 1;
            break;
    }

    return 0;

}
/* Read Register */
/* 5 Points */
// Reads the index from r1 and r2 and gives the data located in each within the array Reg which is stored in data1 and data2.
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2){
    //  Read the registers addressed by r1 and r2 from Reg, and write the read values to data1 and data2 respectively
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}

/* Sign Extend */
void sign_extend(unsigned offset,unsigned *extended_value){ //Fix
    

    unsigned signBit = offset >> 15;
    if(signBit > 0){//if negative sign extend for negative
        *extended_value = offset | 0b11111111111111110000000000000000;
    }else{//extend for an unsigned
        *extended_value = offset & 0b00000000000000001111111111111111;
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero){
   	//data1 and data2
    if(ALUSrc == 0b0){

        if(ALUOp == 0b111){
            switch (funct)
            {   
                //add
                case 0b100000:       //4 0
                    ALUOp = 0b000;
                    break;
                //subtract
                case 0b100010:        //4 2
                    ALUOp = 0b001;
                    break;
                //and
                case 0b100100:        //4 4
                    ALUOp = 0b100;
                    break;
                //or
                case 0b100101:        //4 5
                    ALUOp = 0b101;
                    break;
                //Set less than
                case 0b101010:        //5 2
                    ALUOp = 0b010;
                    break;
                //Set less than unsigned
                case 0b101011:        //5 3
                    ALUOp = 0b011;
                    break;
                //Shift Left Logical
                case 0b000110:       //0 0
                    ALUOp = 0b110;
                    break;
                default:
                    return 1;
                    break;
                }
        }


        ALU(data1,data2,ALUOp,ALUresult,Zero);
        return 0;
    }
    //extended_value
    else if(ALUSrc == 0b1){
        
        ALU(data1,extended_value,ALUOp,ALUresult,Zero);
        return 0;
    }else{
        return 1;
    }
}

/* Read / Write Memory */
/* 10 Points */
// Finds out if we have to write, read, or do nothing.
// If we read we take from ALUresult and put it in memData
// Writing you take the value of data2 and put it in ALUresult
// Return 1 if hault condition
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem){
     if(MemWrite == 1){
        if(((ALUresult % 4)!=0)){
            printf("Write but halt:P\n");
            return 1;
        }
        Mem[ALUresult >> 2] = data2;
    }
    else if(MemRead == 1){
        if(((ALUresult % 4)!=0)){
            printf("Read but halt:P\n");
            return 1;
        }
        *memdata = Mem[ALUresult >> 2];
    }
    
    return 0;
}

/* Write Register */
/* 10 Points */
// Write either ALUresult or memdata into a register (r2 or r3) (May need to look over)
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg){
    if (RegWrite == 0b1 && MemtoReg == 0b1){
        if(RegDst == 0b0){
            Reg[r2] = memdata;
        }
        else if(RegDst == 0b1){
            Reg[r3] = memdata;
        }
    }
    else if(RegWrite == 0b1 && MemtoReg == 0b0){
        if(RegDst == 0b0){
            Reg[r2] = ALUresult;
        }
        else if(RegDst == 0b1){
            Reg[r3] = ALUresult;
        }
    }
}

/* PC update */
/* 10 Points */
// Update pc / program counter
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char
Zero,unsigned *PC)
{
    if(Jump == 0b1){ //checks to if jump control is on
        jsec = jsec << 2;
        unsigned fourUpperBitsPC = *PC & 0b11110000000000000000000000000000;
        *PC = fourUpperBitsPC + jsec;
    }else if(Branch == 0b1 && Zero == 0b1){ //checks to if branch and zero controls are on
        extended_value = extended_value << 2;
        *PC = *PC + extended_value + 4;
    }else{ //increments the PC counter by 4
        *PC = *PC +4;
    }
}
