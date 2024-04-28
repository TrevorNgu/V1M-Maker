struct Filter {
    float firstFil;
    float secFil;
    float thirdFil;
    float fourthFil;
    float fifthFil;
    float RGChrom;
    float BYChrom;
    string debug;
    string debugpix;

    bool operator==(const Filter& rhs) {
        return (firstFil == rhs.firstFil) &&
               (secFil == rhs.secFil) &&
               (thirdFil == rhs.thirdFil) &&
               (fourthFil == rhs.fourthFil) &&
               (fifthFil == rhs.fifthFil);
    }
};