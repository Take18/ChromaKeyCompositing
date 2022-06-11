#include "handle_image.hpp"
#include <cmath>

double calcChromakeyAverage( Image* image, int color );
double calcHue( double r, double g, double b );
double calcSaturation( double r, double g, double b );

/** 倍率変更 */
/** 
 * arguments:
 * 1: human image path
 * 2: chromakey image path
 * 3: background image path
 * 4: d1 recommending 33
 * 5: d2 recommending 3
 * 6: x position
 * 7: y position
 * 8: expantion rate
 * 9: rotation degrees
*/
int main(int argc, char **argv){
    const double pi = 3.141592653589793;
    if ( argc < 10 ) {
        cout << "Too few arguments." << endl;
        return 1;
    }

    Image* human = new Image(string(argv[1]));
    Image* chromakey = new Image(string(argv[2]));
    Image* background = new Image(string(argv[3]));
    int d = atoi(argv[4]), d2 = atoi(argv[5]);
    int x = atoi(argv[6]), y = atoi(argv[7]);
    double expansion_rate = atof(argv[8]);
    double rotation_radian = atof(argv[9]) / 180.0 * pi;
cout << rotation_radian << endl;
    double r_average, g_average, b_average;
    r_average = calcChromakeyAverage(chromakey, RED);
    g_average = calcChromakeyAverage(chromakey, GREEN);
    b_average = calcChromakeyAverage(chromakey, BLUE);
    delete chromakey;
    double chromakey_hue = calcHue(r_average, g_average, b_average);

    Image* expanded_human = new Image(expansion_rate * human->width(), expansion_rate * human->height(), 3);

    double rw = human->width()/(double)(expanded_human->width());
    double rh = human->height()/(double)(expanded_human->height());
    Point v = *(new Point(0, 0, 0));
    for ( int ix=0; ix<expanded_human->width(); ix++ ) {
        for ( int iy=0; iy<expanded_human->height(); iy++ ) {
            for ( int jx=rw*ix; jx<rw*(ix+1); jx++ ) {
                for ( int jy=rh*iy; jy<rh*(iy+1); jy++ ) {
                    double weight = (min(rw*(ix+1), double(jx+1))-max(rw*ix, double(jx))) / rw * (min(rh*(iy+1), double(jy+1))-max(rh*iy, double(jy))) / rh;
                    v += human->pointVals(jx+jy*human->width()) * weight;
                }
            }
            expanded_human->setPointVals(ix+iy*expanded_human->width(), v);
            v.r = 0;
            v.g = 0;
            v.b = 0;
        }
    }

    delete human;

    int expanded_max_point = expanded_human->width() * expanded_human->height();
    for ( int i=0; i<expanded_max_point; i++ ) {
        Point current_point = expanded_human->pointVals(i);
        double current_hue = calcHue(current_point.r, current_point.g, current_point.b);
        if ( chromakey_hue - current_hue < d && chromakey_hue - current_hue > -d && calcSaturation(current_point.r, current_point.g, current_point.b) > d2 ) {
            continue;
        }
        int unrotatedX = i%expanded_human->width();
        int unrotatedY = i/expanded_human->width();
        int rotatedX = unrotatedX*cos(rotation_radian) + unrotatedY*sin(rotation_radian);
        int rotatedY = unrotatedY*cos(rotation_radian) - unrotatedX*sin(rotation_radian);
        int currentX = rotatedX + x;
        int currentY = rotatedY + y;
        int point = currentX + currentY * background->width();
        if ( currentX < background->width() && currentY < background->height() ) {
            background->setPointVals(point, current_point);
        }
    }
    delete expanded_human;

    background->output("images/"+string(argv[6])+"-"+string(argv[7])+"-"+string(argv[8])+"-"+string(argv[9])+".jpg");
    delete background;
    return 0;
}

double calcChromakeyAverage( Image* image, int color ) {
    double ave = 0;
    int max_point = image->width() * image->height();
    for ( int i=0; i<max_point; i++ ) {
        if ( color == RED ) {
            ave += image->pointVals(i).r / (double)max_point;
        } else if ( color == GREEN ) {
            ave += image->pointVals(i).g / (double)max_point;
        } else if ( color == BLUE ) {
            ave += image->pointVals(i).b / (double)max_point;
        }
    }
    return ave;
}

double calcHue( double r, double g, double b ) {
    double h = 0;
    if ( r == g && g == b ) return 0.0;

    double max_val = max(max(r, g), b);
    if ( max_val == r ) {
        h = 60 * ( (g - b) / (r - min(g, b)) );
    } else if ( max_val == g ) {
        h = 60 * ( (b - r) / (g - min(b, r)) ) + 120;
    } else {
        h = 60 * ( (r - g) / (b - min(r, g)) ) + 240;
    }
    return h>0 ? h : h + 360;
}

double calcSaturation( double r, double g, double b ) {
    double max_val = max(max(r, g), b);
    double min_val = min(min(r, g), b);
    double cnt = (max_val + min_val) / 2.0;
    double s = (max_val - min_val) / (cnt>=128 ? (max_val+min_val) : (510-max_val-min_val));
    return s * 100;
}