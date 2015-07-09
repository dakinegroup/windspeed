static char binaryString[50];
char * getBinaryString(unsigned int bytes) {
    int i = 0;
    for(;i<16; i++) {
        binaryString[i*2] =  ((bytes << i)&0x8000)?'1':'0';
        binaryString[i*2+1] = ' ';
    }
    binaryString[i*2] = 0;
    return &binaryString[0];
}