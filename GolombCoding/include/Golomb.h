#include <vector>

class Golomb{
    private:
        int m;
        int bits;
        bool reminderSizePlus1 = false;
    public:
        Golomb(int n);
        std::vector<int>* encodeNumber(int);
        int decodeNumber(int*,int);
        int getLastReminderSizePlus1();
};