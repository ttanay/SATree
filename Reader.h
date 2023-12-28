#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

// Source: http://corpus-texmex.irisa.fr/

class BaseVecsReader
{
    int n;

protected:
    FILE * fptr;
    static const int DIM_SIZE = 4;
    int d;
    int curr{0};

    BaseVecsReader(char * filename)
    {
        fptr = fopen(filename, "rb");
        if (fptr == NULL)
        {
            printf("Error: %d (%s)\n", errno, strerror(errno));
            exit(1);
        }
        fread(&d, sizeof(int), 1, fptr);

        fseek(fptr, 0, SEEK_END);
        int size = ftell(fptr);
        n = size / ((1 + d) * DIM_SIZE);

        fseek(fptr, 0, SEEK_SET);
    }
public:

    ~BaseVecsReader() { fclose(fptr); }

    bool eof() { return feof(fptr) || curr >= n; }

    inline int num_vectors() { return n; }

    // Skip the first 4 bytes that hold the dimension
    inline void skip_dim() { fseek(fptr, DIM_SIZE, SEEK_CUR); }

    inline int dimension() { return d; }
};

class FVecsReader: public BaseVecsReader
{
public:
    FVecsReader(char *filename_): BaseVecsReader(filename_) {}

    std::vector<float> readvec()
    {
        if(eof())
            throw std::runtime_error("EOF reached");

        std::vector<float> res(d);
        skip_dim();
        fread(&res, sizeof(float), DIM_SIZE, fptr);
        curr++;
        return res;
    }
};

class IVecsReader: public BaseVecsReader
{
public:
    IVecsReader(char *filename_): BaseVecsReader(filename_) {}

    std::vector<int> readvec()
    {
        if(eof())
            throw std::runtime_error("EOF reached");

        std::vector<int> res(d);
        skip_dim();
        fread(&res, sizeof(int), DIM_SIZE, fptr);
        curr++;
        return res;
    }
};