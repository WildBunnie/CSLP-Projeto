#include <vector>

/**
 * \brief A entropy encoder built with Golomb codes.
 *
 */
class Golomb{
    private:
        /**
         * \brief Parameter used to divide the number to be encoded.
         *
         */
        int m;
        /**
         * \brief Number of bits to be used in the formula.
         *
         */
        int bits;
        /**
         * \brief Used to know if the number of bits used was "bits" or bits + 1
         *
         */
        bool reminderSizePlus1 = false;
    public:
        /**
         * \brief Constructor.
         *
         * \param n Parameter used to divide the number to be encoded.
         */
        Golomb(int n);
        /**
         * \brief Encodes number.
         *
         * \param number Number to be encoded.
         * \return Number encoding.
         */
        std::vector<int>* encodeNumber(int);
        /**
         * \brief Decodes number.
         *
         * \param numbers Number encoding.
         */
        int decodeNumber(int*);
        /**
         * \brief Gets the value of reminderSizePlus1 variable converted to integer.
         *
         * \return 1 if the number of bits is equal to bits + 1 and 0 if not.
         */
        int getLastReminderSizePlus1();
};