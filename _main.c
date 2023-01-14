#include <stdio.h>
#include<mlx.h>

typedef struct	s_data {
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
}				t_data;


void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

int main(void)
{
    void* mlx = mlx_init();
    void* win = mlx_new_window(mlx, 1080, 720, "test");

    t_data  img;
    //  img.img = mlx_new_image(mlx, 640, 720);
    // img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
								// &img.endian);

    // my_mlx_pixel_put(&img, 100, 100, 0xffffff);
    // my_mlx_pixel_put(&img, 101, 100, 0xffffff);
    // my_mlx_pixel_put(&img, 102, 100, 0xffffff);
    // mlx_put_image_to_window(mlx, win, img.img, 0, 0);
    mlx_loop(mlx);
}