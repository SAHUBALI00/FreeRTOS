void* memset(void *dst, int value, unsigned int size) {
    unsigned char *ptr = (unsigned char *)dst;
    unsigned char byte_value = (unsigned char)value;

    // Fill memory byte by byte
    while (size > 0) {
        *ptr = byte_value;
        ptr++;
        size--;
    }

    return dst; // Return original pointer as per standard
}
void* memcpy(void *dest, const void *src, unsigned int n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}