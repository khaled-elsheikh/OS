void printString(char*);
void readString(char*);
void readSector(char*, int);
void handleInterrupt21(int, int, int, int);
int mod(int, int);
int div(int, int);
void readFile(char*, char*);
void executeProgram(char*, int);
void terminate();
void writeSector(char*, int);
void deleteFile(char*);
void writeFile(char* , char* , int);

int main()
{
    //shell:
    //makeInterrupt21();
    //interrupt(0x21, 4, "shell\0", 0x2000, 0);
    int i=0;
    char buffer1[13312];
    char buffer2[13312];
    buffer2[0]="h"; buffer2[1]="e"; buffer2[2]="l"; buffer2[3]="l";
    buffer2[4]="o";
    for(i=5; i<13312; i++) buffer2[i]=0x0;
    makeInterrupt21();
    interrupt(0x21,8, "testW\0", buffer2, 1); //write file testW
    interrupt(0x21,3, "testW\0", buffer1, 0); //read file testW
    interrupt(0x21,0, buffer1, 0, 0); // print out contents of testW
    while(1);
}

void printString(char* chars){
	int i = 0;
 	while(chars[i] != '\0'){	
 	interrupt(0x10, 0xE*256+chars[i], 0, 0, 0);
 	i++;
 	}
 }
 void readString(char* arr){ 
        char enter = 0xd; 
        char endLine = 0xa; 
        char nullChar = 0x0; 
        char back = 0x8; 


        int i = 0; 
        char ascii = interrupt(0x16, 0, 0, 0, 0);
        interrupt(0x10, 0xe*256+ascii, 0, 0, 0);


        while(ascii!=enter){ 

        
            if(ascii==back&&i>0) {i--;}
            else if(ascii==back) {i=0;} 
            else {arr[i]=ascii; i++;}   


            ascii = interrupt(0x16, 0, 0, 0, 0); 
            interrupt(0x10, 0xe*256+ascii, 0, 0, 0);


            if(ascii==back){
                interrupt(0x10, 0xe*256+nullChar, 0, 0, 0);
                interrupt(0x10, 0xe*256+ascii, 0, 0, 0);
            }
        }

        arr[i] = endLine; 
        arr[i+1] = nullChar; 


        
        interrupt(0x10, 0xe*256+endLine, 0, 0, 0); 
    }
    void readSector(char* buffer, int sector){ 
        int ah = 2;
        int al = 1;
        int ax = ah * 256 + al; 
        int bx = buffer;
        int ch = div(sector,36); 
        int cl = mod(sector,18)+1;
        int cx = ch * 256 + cl;     
        int dh = mod(div(sector,18),2);
        int dl = 0;
        int dx = dh * 256 + dl; 

        

        interrupt(0x13, ax, bx, cx, dx); 
      
    } 
    int mod(int a, int b){ 
        while(a>=b)
            a=a-b;
        return a; 
    } 


    int div(int a, int b){ 
        int q = 0; 
        while (a>=b){
            q++;
            a=a-b;
        }
        return q; 
    } 

void handleInterrupt21(int ax, int bx, int cx, int dx)
{ 
        if(ax==0)
        {            
            printString(bx); 
        } 
        else if(ax==1) 
        {            
            readString(bx);
        }
        else if(ax==2) 
        {            
            readSector(bx,cx); 
        }
        else if (ax == 3) 
        {
            readFile(bx, cx);
        }
        else if(ax==4) 
        {
            executeProgram(bx , cx);
        }
        else if(ax==5)
        {
            terminate();
        }
        else if(ax == 6)
        {
            writeSector(bx, cx);
        }
        else if(ax == 7)
        {
            deleteFile(bx);
        }
        else if(ax == 8)
        {
            writeFile(bx, cx , dx);
        }else
        {
            printString("Invalid use of interrupt 21\0"); 
        }

    } 
 	
void readFile (char* fileName, char* buffer)
{
    int i = 0;
    int j = 0;
    int found = 0;
    char directory[512];
    readSector(directory, 2);
    while(i < 512)
    {
        j = 0;
        while (j < 6)
        {
            if (fileName[j] != directory[i])
            {
                i++;
                break;
            }
            i++;
            j++;
            if (j == 6)
                found = 1;
        }
        if (found)
            break;
    }
    if (!found)
        return;
    j = 0;
    while(j < 26)
    {
        if (directory[i] == 0x00)
            break;
        readSector(buffer, directory[i]);
        i++;
        buffer += 512;
    }
}

void executeProgram(char* name, int segment){
    char buffer[13312];
    int i = 0;
    readFile(name , buffer);
    while(i < 13312){
        putInMemory(segment , i, buffer[i]);
        i++;
    }
    launchProgram(segment);
}

void terminate(){
    char shell[6];
    shell[0] = 's';
    shell[1] = 'h';
    shell[2] = 'e';
    shell[3] = 'l';
    shell[4] = 'l';
    shell[5] = '\0';
    interrupt(0x21, 4, shell, 0x2000, 0);
}

void writeSector(char* buffer, int sector)
{
    interrupt(0x13, 3 * 256 + 1, buffer, div(sector, 36) * 256 + (mod(sector, 18) + 1), mod(div(sector, 18), 2) * 256);
}

void deleteFile(char* name)
{
    char map[512];
    char directory[512];
    int i = 0;
    int j = 0;
    int found = 0;

    readSector(map, 1);
    readSector(directory, 2);

    while(i < 512)
    {
        j = 0;
        while (j < 6)
        {
            if (name[j] != directory[i])
            {
                i++;
                break;
            }
            i++;
            j++;
            if (j == 6){
                found = 1;
            }
        }
        if (found == 1)
        {
            break;
        }
    }
    if (found == 0)
    {
        printString("File not found.\0");
        return;
    }
    i -= 6;
    directory[i] = 0x00;
    i += 6;
    j = 0;

    while(j < 26)
    {
        if (directory[i] == 0x00)
            break;
        map[directory[i] + 1] = 0x00;
        i++;
        j++;
    }

    writeSector(map, 1);
    writeSector(directory, 2);
}

void writeFile(char* name, char* buffer, int secNum){
    char map[512];
    char directory[512];
    char buffer2[512];
    int i = 0;
    int j = 0;
    int found = 0;
    int x = 0;

    readSector(map, 1);
    readSector(directory, 2);
    while(i < 512){
        if (directory[i] == 0x00){
            found = 1;
            break;
        }
        i++;
    }
    while(*name != '\0')
    {
        directory[i] = *name; 
        name++;
        i++;
        j++;
    }
    if(j < 6){
        while(j < 6){
            directory[i] = 0x00;
        }
    }
    if (found == 0)
    {
        printString("could not write the file.\0");
        return;
    }

    i -= 6;
    directory[i] = 0x00;
    i += 6;
    j = 0;
    for(x = 0; x < secNum ; x++){
        if(map[x] == 0x00){
            map[x] = 0xFF;
            directory[i] = secNum;
            while(j < 512){
                buffer2[j] = buffer[j*(x+1)];
                j++;
            }
            writeSector(buffer2,x);
        }
    }


    writeSector(map,1);
    writeSector(directory,2);


}