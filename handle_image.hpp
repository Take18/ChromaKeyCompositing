#include <iostream>
#include <algorithm>
#include <string>
#include "jpegio.hpp"
#define MAX_FILENAME_LENGTH 128
#define RED 0
#define GREEN 1
#define BLUE 2
using namespace std;

class Point {
    public:
        int r;
        int g;
        int b;
        Point(int r, int g, int b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        Point& operator+ (Point& p){
            return *(new Point(this->r + p.r, this->g + p.g, this->b + p.b));
        }

        Point& operator+= (Point& p){
            this->r += p.r;
            this->g += p.g;
            this->b += p.b;
            return *this;
        }

        Point& operator* (double x){
            return *(new Point(this->r*x, this->g*x, this->b*x));
        }
};

class Image {
    public:
        string image_name;
        BYTE* image_data;
        int image_data_size;
        int image_width;
        int image_height;
        int image_bpp;

        Image(string image_name) {
            this->image_name = image_name;
            this->image_data_size = readJpeg(this->image_name, &(this->image_data));
            if ( this->image_data_size == 0 ) {
                cout << "could not read " << this->image_name << endl;
                exit(EXIT_FAILURE);
            }
            this->image_width = readHeader(this->image_name, IMAGE_WIDTH);
            this->image_height = readHeader(this->image_name, IMAGE_HEIGHT);
            this->image_bpp = readHeader(this->image_name, IMAGE_BPP);
        }

        Image(int width, int height, int bpp) {
            this->image_width = width;
            this->image_height = height;
            this->image_bpp = bpp;
            this->image_data_size = width * height * bpp;
            this->image_data = new BYTE[this->image_data_size];
        }

        Image(Image* base_image) {
            this->image_width = base_image->width();
            this->image_height = base_image->height();
            this->image_bpp = base_image->bpp();
            this->image_data_size = base_image->size();
            this->image_data = new BYTE[this->image_data_size];
        }

        ~Image() {
            delete this->image_data;
        }

        int width() {
            return this->image_width;
        }

        int height() {
            return this->image_height;
        }

        int bpp() {
            return this->image_bpp;
        }

        int size() {
            return this->image_data_size;
        }

        Image* output(string image_name = "", int quality = 100) {
            writeJpeg((image_name != "") ? image_name : this->image_name, this->image_data, this->width(), this->height(), this->bpp(), quality);
            if ( image_name != "" ) {
                Image* ret_image = new Image(this);
                ret_image->image_name = image_name;
                return ret_image;
            }
            return this;
        }

        void setPointVals(int i, Point& p) {
            if ( this->bpp() == 3 ) {
                this->image_data[i*3  ] = p.r;
                this->image_data[i*3+1] = p.g;
                this->image_data[i*3+2] = p.b;
            } else {
                this->image_data[i] = (p.r + p.g + p.b) / 3.0;
            }
        }

        void setPointVals(int i, int r, int g, int b) {
            if ( this->bpp() == 3 ) {
                this->image_data[i*3  ] = r;
                this->image_data[i*3+1] = g;
                this->image_data[i*3+2] = b;
            } else {
                this->image_data[i] = (r + g + b) / 3.0;
            }
        }

        void setPointVals(int i, int val) {
            if ( this->bpp() == 3 ) {
                this->image_data[i*3  ] = val;
                this->image_data[i*3+1] = val;
                this->image_data[i*3+2] = val;
            } else if ( this->bpp() == 1 ) {
                this->image_data[i] = val;
            }
        }

        Point& pointVals(int i) {
            if ( this->bpp() == 3 ) {
                return *(new Point(
                    this->image_data[i*3],
                    this->image_data[i*3+1],
                    this->image_data[i*3+2]
                ));
            } else {
                return *(new Point(
                    this->image_data[i],
                    this->image_data[i],
                    this->image_data[i]
                ));
            }
        }
};
