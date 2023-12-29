#pragma once

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

public:
    ~BaseVecsReader() { if(fptr) close(); }
    BaseVecsReader() = default;
    BaseVecsReader(char *filename) { open(filename); }
    void open(char * filename)
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

    void close() {
        fclose(fptr);
        curr = 0;
    }

    bool eof() { return feof(fptr) || curr >= n; }

    inline int vec_id() { return curr; }

    inline int num_vectors() { return n; }

    // Skip the first 4 bytes that hold the dimension
    inline void skip_dim() { fseek(fptr, DIM_SIZE, SEEK_CUR); }

    inline int dimension() { return d; }
};

class FVecsReader: public BaseVecsReader
{
public:
    float* readvec()
    {
        if(eof())
            throw std::runtime_error("EOF reached");

        float *arr = new float[d];
        skip_dim();
        fread(arr, sizeof(float), d, fptr);
        curr++;
        return arr;
    }
};

class IVecsReader: public BaseVecsReader
{
public:
    int *readvec()
    {
        if(eof())
            throw std::runtime_error("EOF reached");

        int *arr = new int[d];
        skip_dim();
        fread(arr, sizeof(int), d, fptr);
        curr++;
        return arr;
    }
};