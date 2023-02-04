#include<mlx.h>
#include<stdio.h>

typedef struct	s_img_data {
	void	*img;
	int	    *addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
	int		texture_width;
	int		texture_height;
}				t_img_data;

int main()
{
    void    *mlx = mlx_init();
    void    *wid = mlx_new_window(mlx, 1920, 1080, "test");
    t_img_data img;
    int width;
    int height;
    img.img = mlx_xpm_file_to_image(mlx, "assests/t.xpm", &img.texture_width, &img.texture_height);
    img.addr = (int *) mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
    
    printf("%d .  %d\n", width, height);
    for (int y = 0; y < 1080; ++y){
        for (int x = 0; x < 1920 ; ++x){
            int* dst = (int *)((char *)img.addr + (y * img.texture_width + x ) * (img.bits_per_pixel / 8));
            // printf("%d\n", *(unsigned int *)dst);
            mlx_pixel_put(mlx, wid, x, y, *dst);
        }
    }
    mlx_loop(mlx);
}


void    rendering_3d_walls(t_cub_manager* manager)
{
    // clear_window(manager, 0x000000, WIDTH, HEIGHT);
        int wi, hi, bbp, line, endian;
        void *img = mlx_xpm_file_to_image(manager->mlx_manager.mlx, "/Users/yakhoudr/cube3d/assets/df.xpm", &wi, &hi);
        char *addr = mlx_get_data_addr(img, &bbp, &line, &endian);
        for (int i = 0; i < NUMBER_OF_RAYS; i++) {
            // printf("distance: %f\n", manager->rays[i].distance);
        double perpDistance = manager->rays[i].distance * cos(manager->rays[i].rayAngle - manager->player.rotation_angle);
        // printf("%lf, %lf, (%s:%d)\n", manager->rays[i].distance, cos(manager->rays[i].rayAngle - manager->player.rotation_angle), __FILE__, __LINE__);
        double distanceProjPlane = (WIDTH / 2.0) / tan(radians(FOV) / 2.0);
        double projectedWallHeight = ((double)TILE_SIZE / perpDistance) * distanceProjPlane;

        int wallStripHeight = (int)projectedWallHeight;

        int wallTopPixel = (HEIGHT / 2.0) - (wallStripHeight / 2.0);
        // wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

        int wallBottomPixel = (HEIGHT / 2.0) + (wallStripHeight / 2.0);
        // wallBottomPixel = wallBottomPixel > HEIGHT ? HEIGHT : wallBottomPixel;

        double offx;
        if (manager->rays[i].wasHitVertical)
        {
            offx = fmod(manager->rays[i].wallHitY, TILE_SIZE); 
        }
        else
            offx = fmod(manager->rays[i].wallHitX, TILE_SIZE); 
        offx = offx / TILE_SIZE * wi;
        // render the wall from wallTopPixel to wallBottomPixel
        for (int j = wallTopPixel; j < wallBottomPixel;++j)
        {
            double offy = (j - wallTopPixel) * (hi / (double)wallStripHeight);
            cub_mlx_pixel_put(&manager->mlx_manager.img_data, i * WALL_STRIP_WIDTH, j, WIDTH, HEIGHT, (int) *((int *)(addr + ((int)offy * line + (int)offx * (bbp / 8)))));
        }
    }
    
}