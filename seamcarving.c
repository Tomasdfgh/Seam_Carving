#include "seamcarving.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "c_img.h"
#include <string.h>

int get_x(struct rgb_img *im, int x, int y, int col){
    if(x==0){
        int x_add = get_pixel(im, y, 1, col);
        int x_minus = get_pixel(im,y,im->width - 1,col);
        return x_add - x_minus;
    }
    if(x == im->width - 1){
        int x_add = get_pixel(im, y, 0, col);
        int x_minus = get_pixel(im, y,(im->width - 2), col);
        return x_add - x_minus;
    }else{
        int x_add = get_pixel(im,y,x+1,col);
        int x_minus = get_pixel(im,y,x-1,col);
        return x_add - x_minus;
    }
}

int get_y(struct rgb_img *im, int x, int y, int col){
    if(y == 0){
        int y_add = get_pixel(im,1,x,col);
        int y_minus = get_pixel(im,(im->height -1),x,col);
        return y_add - y_minus;
    }
    if(y == im->height - 1){
        int y_add = get_pixel(im,0,x,col);
        int y_minus = get_pixel(im,(im->height-2),x,col);
        return y_add - y_minus;
    }else{
        int y_add = get_pixel(im,y+1,x,col);
        int y_minus = get_pixel(im,y-1,x,col);
        return y_add - y_minus;
    }
}

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    create_img(grad,im->height,im->width);
    for(int x = 0; x < im->width; x++){
        for(int y = 0; y < im->height; y++){
            int red_x = get_x(im,x,y,0);
            int red_y = get_y(im,x,y,0);
            int blue_x = get_x(im,x,y,2);
            int blue_y = get_y(im,x,y,2);
            int green_x = get_x(im,x,y,1);
            int green_y = get_y(im,x,y,1);
            int d_two_x = pow(red_x,2) + pow(green_x,2) + pow(blue_x,2);
            int d_two_y = pow(red_y,2) + pow(blue_y,2) + pow(green_y,2);
            int ans = pow(d_two_x + d_two_y, 0.5);
            ans = ans/10;
            uint8_t ans_2 = (uint8_t)ans;
            set_pixel((*grad),y,x,ans_2,ans_2,ans_2);
        }
    }
}

int min_2(int a, int b){
    if(a < b){
        return a;
    }
    if(b<a){
        return b;
    }else{
        return a;
    }
}

int min_3(int a, int b, int c){
    if (a < b && a < c){
        return a;
    }
    if (b < a && b < c){
        return b;
    }
    if (c < a && c < b){
        return c;
    }
    if (a == b && a < c){
        return a;
    }
    if (b == c && b < a){
        return b;
    }else{
        return a;
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr){
    *best_arr = (double *)malloc(grad->height * grad->width * sizeof(double));
    for(int i = 0; i < grad->width; i++){
        (*best_arr)[i] = get_pixel(grad,0,i,0);
    }
    for(int x = grad->width; x < (grad->width * grad->height); x++){
        int y = (x - (x % grad->width))/grad->width;
        if(x % grad->width == 0){
            //Pick two on the two left side
            int first = (*best_arr)[x - grad->width] + get_pixel(grad,y,x - y*grad->width,0);
            int second = (*best_arr)[x - grad->width + 1] + get_pixel(grad, y,x - y*grad->width,0);
            int selected = min_2(first, second);
            (*best_arr)[x] = selected;
        }
        else if(x + 1 % grad->width == 0){
            //Pick two on the two right side
            int first = (*best_arr)[x - grad->width - 1] + get_pixel(grad,y,x - y*grad->width,0);
            int second = (*best_arr)[x - grad->width] + get_pixel(grad,y,x - y*grad->width,0);
            int selected = min_2(first, second);
            (*best_arr)[x] = selected;
        }else{
            //Pick the best three
            int first = (*best_arr)[x - grad->width - 1] + get_pixel(grad,y,x - y*grad->width,0);
            int second = (*best_arr)[x - grad->width] + get_pixel(grad,y,x - y*grad->width,0);
            int last = (*best_arr)[x - grad->width + 1] + get_pixel(grad,y,x - y*grad->width,0);
            int selected = min_3(first,second,last);
            (*best_arr)[x] = selected;
        }
    }
}

void recover_path(double *best, int height, int width, int **path){
    *path = (int *)malloc(height * sizeof(int));
    int cur = best[height*width - width];
    int s_p = height*width - width;
    int s_p_2 = 0;
    int counter = 1;
    for(int x = height*width - width; x < height*width; x++){
        if(best[x] <= cur){
            s_p = x;
            cur = best[x];
        }
        (*path)[0] = s_p % width;
    }
    s_p = s_p - width;
    s_p_2 = s_p;
    for(int y = height - 1; y > 0; y--){
        for(int x_2 = s_p - 1; x_2 <= s_p + 1; x_2++){
            if(best[x_2] <= cur){
                cur = best[x_2];
                s_p_2 = x_2;
            }
        }
        s_p = s_p_2 - width;
        (*path)[counter] = s_p_2 % width;
        counter++;
    }
    flip(*path, height);
}

void flip(int *path, int height)
{
    int temp = 0;
    for(int i = 0; i < (int)(height/2); i++){
        temp = (path)[i];
        path[i] = path[height - i - 1];
        path[height-i -1] = temp;
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    create_img(dest, src->height,src->width - 1);
    for(int y = 0; y < src -> height; y++){
        int skip = 0;
        for(int x = 0; x < src ->width; x++){
            if(path[y] == x){
                skip = 1;
                continue;
            }
            uint8_t red = get_pixel(src, y, x, 0);
            uint8_t green = get_pixel(src, y, x, 1);
            uint8_t blue = get_pixel(src, y, x, 2);
            set_pixel(*dest, y, x - skip, red, green, blue);   
        }
    }
}