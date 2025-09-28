void kmain(void) {
    char *video = (char*)0xb8000;
    const char *msg = "System44";
    for(int i=0; msg[i]; i++){
        video[i*2] = msg[i];
        video[i*2+1] = 0x07;
    }
    for(;;);
}