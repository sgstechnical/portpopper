

//##############################################################################
//@ String helpers
//##############################################################################

//Sets the first char of a buffer to 0
void string_clear(char* str){
    if (str){
        str[0] = 0;
    }
}


//Copies a string from s to d, upto the null or destMax
size_t string_copy(char* d, const char* s, size_t destMax){
    
    size_t count = 0;
    if (!s) {
        *d = 0;
        return 0;
    }
    
    while(*s && count < destMax-1){
        *d++ = *s++;
        count ++;
    }
    *d = NULL;
    return count;
}


//Gets the string length of 'string',upto but not including the NULL
size_t string_length(const char* string){
    
    size_t length = 0;
    if (string == NULL) return 0;
    while(*string++){
        length ++;
    }
    
    return length;
}


//##############################################################################
//@ Memory helpers
//##############################################################################


void memory_clear(void*p,size_t s){
    for (size_t i = 0; i<s; i++){
        ((uint8_t*)p)[i] = 0x00;
    }
}

void memory_copy(void* dest, void* src, int64_t length){
    int i = 0;
    while (i<length){
        ((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
        ++i;
    }
}



//end
