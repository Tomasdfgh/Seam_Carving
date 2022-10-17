#include "c_img.h"
#include <stdio.h>
#include <math.h>
#include "seamcarving.h"

int main(void){
    struct rgb_img *im;
    struct rgb_img *cur_im;
    struct rgb_img *grad;
    double *best;
    int *path;

    read_in_img(&im, "img35.bin");
    
    for(int i = 0; i < 100; i++){
        calc_energy(im,  &grad);
        //printf("Energy:\n");
        //printf("Dynamic Seam:\n");
        dynamic_seam(grad, &best);
        //for(int y = 0; y < grad->height; y++){
        //    printf("\n");
        //    for(int x = 0; x < grad->width; x++){
        //        
        //        printf("%f\t", best[y*grad->width + x]);
        //    }
        //}
        recover_path(best, grad->height, grad->width, &path);
        //printf("\n");
        //printf("Path:\n");
        //for(int i = 0; i < grad->height; i++){
        //    printf("%d", path[i]);
        //}
        //printf("\n");
        remove_seam(im, &cur_im, path);
        char filename[200];
        if (i  % 5 == 0){
            sprintf(filename, "img%d.bin", i);
        }
        write_img(cur_im, filename);

        free(best);
        free(path);
        im = cur_im;
    }
    destroy_image(im);
}