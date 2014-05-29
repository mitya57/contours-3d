/* index should be in [0..5] range */
void getTetrahedByIndex(char index, char tnumbers[4]) {
    tnumbers[0] = 0;
    tnumbers[1] = 7;
    tnumbers[2] = 1 << (2 - (index % 3));
    tnumbers[3] = tnumbers[2] + (1 << (index >> 1));
}

/* index should be in [0..3] range */
void getTetrahedVerticeByIndex(char index, char const tnumbers[4], char vnumbers[3]) {
    vnumbers[0] = (index > 0) ? tnumbers[0] : tnumbers[1];
    vnumbers[1] = (index > 1) ? tnumbers[1] : tnumbers[2];
    vnumbers[2] = (index > 2) ? tnumbers[2] : tnumbers[3];
}
