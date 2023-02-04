#include "../include/cub3d.h"
void	rendering_3d_walls(t_cub_manager* manager);

void	cast_all_rays(t_cub_manager* manager);

double radians(double angle)
{
	return (angle * (M_PI / (double)180));
}

inline double dist(int x1, int x2, int y1, int y2)
{
    // Calculating distance
    return sqrt(pow(x2 - x1, 2)
                + pow(y2 - y1, 2) * 1.0);
}

void	cub_mlx_pixel_put(t_img_data *data, int x, int y, int limit_x, int limit_y, int color)
{
	char	*dst;

	if (x < 0 || x >= limit_x || y < 0 || y >= limit_y)
		return ;
	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

int max(int a, int b)
{
    if (a > b)
        return a;
    else
	    return b;
}
int min(int a, int b)
{
    if (a > b)
        return b;
    else
	    return a;
}

void draw_line(double x0, double y0, double x1, double y1, int lx, int ly, t_cub_manager *manager, int color)
{
  // Calculate the difference between the starting and ending x and y coordinates
  double dx = x1 - x0;
  double dy = y1 - y0;

  // Determine the number of steps required to draw the line
  double steps = fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy);
  int st = round(steps);

  // Calculate the x and y increments for each step
  double xIncrement = dx / (steps);
  double yIncrement = dy / (steps);

  // Set the starting x and y coordinates
  double x = x0;
  double y = y0;

  // Loop through the number of steps required to draw the line
  for (int i = 0; i <= st; ++i) {
    // Set the color of the pixel at (x, y)
    cub_mlx_pixel_put(&manager->mlx_manager.img_data ,round(x), round(y), lx, ly, color);

    // Increment the x and y coordinates
    x += xIncrement;
    y += yIncrement;
  }
}


void normalize_angle(double *ang)
{
	if (*ang > 2 * M_PI)
		*ang -= 2 * M_PI;
	if (*ang < 0)
		*ang += 2 * M_PI;
}

typedef struct s_dpair
{
	double x;
	double y;
	struct s_dpair *next;
}	t_dpair;

t_dpair *create_dpair(double x, double y)
{
	t_dpair *dpair = (t_dpair *)malloc(sizeof(t_dpair));
	if (!dpair)
		exit(1);
	dpair->x = x;
    dpair->y = y;
	dpair->next = 0x0;
	return dpair;
}

void	dpair_add_back(t_dpair **head, t_dpair *new)
{
	if (!head || !new)
		return ;
	if (!*head)
		*head = new;
	else
	{
		t_dpair *p = *head;
		while (p->next)
		{
			p = p->next;
		}
		p->next = new;
	}
}

void clear_window(t_cub_manager * manager, int color, int lx, int ly)
{
	int i = -1;
	while (++i < ly)
	{
		int j = -1;
		while (++j < lx)
		{
			cub_mlx_pixel_put(&manager->mlx_manager.img_data, j, i, lx, ly, color);
		}

	}
}

long	get_map_width(t_map_manager *map_manager)
{
	long	res;
	int		i;
	long	line_size;

	res = 0;
	i = 0;
	while (map_manager->map[i])
	{
		line_size = ft_strlen(map_manager->map[i]);
		if (line_size > res)
			res = line_size;
		++i;
	}
	return (res);
}

long	get_map_height(t_map_manager *map_manager)
{
	long	i;

	i = 0;
	while (map_manager->map[i])
		++i;
	return (i);
}

void	draw_full_rect(int x, int y, int width, int height, int lx, int ly, t_cub_manager *manager, int color)
{
	int i = -1;
	while(++i < height)
		draw_line(x , y + i, x + width, y + i, lx, ly, manager, color);
}

void	draw_empty_rect(int x, int y, int width, int height, int lx, int ly, t_cub_manager *manager, int color)
{
	draw_line(x, y, x + width, y, lx, ly, manager, color);
	draw_line(x, y, x, y + height, lx, ly, manager, color);
	draw_line(x, y + height, x + width, y + height,lx, ly, manager, color);
	draw_line(x + width, y, x + width, y + height,lx, ly, manager, color);
}

int controls(int key, t_cub_manager	*manager)
{
	double fx;
	double fy;
	if (key == 124)
	{
		manager->player.turn_direction = 1;
		normalize_angle(&manager->player.rotation_angle);
		manager->player.rotation_angle += manager->player.turn_direction * manager->player.rotation_speed;
		normalize_angle(&manager->player.rotation_angle);
	}
	else if (key == 123)
	{
		manager->player.turn_direction = -1;
		normalize_angle(&manager->player.rotation_angle);
		manager->player.rotation_angle += manager->player.turn_direction * manager->player.rotation_speed;
		normalize_angle(&manager->player.rotation_angle);
	}
	else if (key == 125)
	{
		manager->player.walk_direction = -1;
		fx = manager->player.x + manager->player.walk_speed * cos(manager->player.rotation_angle) * manager->player.walk_direction * 1.0;
		fy = manager->player.y + manager->player.walk_speed * sin(manager->player.rotation_angle) * manager->player.walk_direction * 1.0;
		if (fy >= 0 && fy < manager->map->map_height * TILE_SIZE && fx >= 0 && fx < manager->map->map_width * TILE_SIZE && manager->map->map[(int)fy / TILE_SIZE][(int)fx / TILE_SIZE] != '1')
		{
			if (!(manager->map->map[(int)fy / TILE_SIZE][(int)manager->player.x / TILE_SIZE] == '1' || manager->map->map[(int)manager->player.y / TILE_SIZE][(int)fx / TILE_SIZE] == '1'))
			{
				manager->player.x += manager->player.walk_speed * cos(manager->player.rotation_angle) * manager->player.walk_direction * 1.0;
				manager->player.y += manager->player.walk_speed * sin(manager->player.rotation_angle) * manager->player.walk_direction * 1.0;
			}
			// printf("hop\n");
		}
		// printf("hooray\n");

	}
	else if (key == 126)
	{
		manager->player.walk_direction = 1;
		fx = manager->player.x + manager->player.walk_speed * cos(manager->player.rotation_angle) * manager->player.walk_direction * 1.0;
		fy = manager->player.y + manager->player.walk_speed * sin(manager->player.rotation_angle) * manager->player.walk_direction * 1.0;
		if (fy >= 0 && fy < manager->map->map_height * TILE_SIZE && fx >= 0 && fx < manager->map->map_width * TILE_SIZE && manager->map->map[(int)fy / TILE_SIZE][(int)fx / TILE_SIZE] != '1')
		{
			if (!(manager->map->map[(int)fy / TILE_SIZE][(int)manager->player.x / TILE_SIZE] == '1' || manager->map->map[(int)manager->player.y / TILE_SIZE][(int)fx / TILE_SIZE] == '1'))
			{
				manager->player.x += manager->player.walk_speed * cos(manager->player.rotation_angle) * manager->player.walk_direction * 1.0;
				manager->player.y += manager->player.walk_speed * sin(manager->player.rotation_angle) * manager->player.walk_direction * 1.0;
			}
			// printf("hop\n");
		}
		// printf("hooray\n");
	}
	if (key == 53)
		exit(EXIT_SUCCESS);
	draw(manager);
	return 0;
}


// void draw_minimap(int player_x, int player_y, int map[][MAP_WIDTH], t_cub_manager *manager)
// {
//     // Clear the mini-map
//     clear_window(manager, 0x000000);

//     // Calculate the starting and ending x and y coordinates for the mini-map
//     int start_x = max(player_x - MINIMAP_WIDTH / 2, 0);
//     int end_x = min(player_x + MINIMAP_WIDTH / 2, MAP_WIDTH);
//     int start_y = max(player_y - MINIMAP_HEIGHT / 2, 0);
//     int end_y = min(player_y + MINIMAP_HEIGHT / 2, MAP_HEIGHT);

//     // Draw the mini-map
//     for (int i = start_y; i < end_y; i++)
//     {
//         for (int j = start_x; j < end_x; j++)
//         {
//             if (map[i][j] == 1)
//                 cub_mlx_pixel_put(&manager->mlx_manager.img_data, MINIMAP_X + j - start_x, MINIMAP_Y + i - start_y, 0xFFFFFF);
//         }
//     }

//     // Draw the player on the mini-map
//     cub_mlx_pixel_put(&manager->mlx_manager.img_data, MINIMAP_X + player_x - start_x, MINIMAP_Y + player_y - start_y, 0xFF0000);
// }
# define mini_x (10)
# define mini_y (10)

void	draw_empty_circle(double x, double y, double radius, int lx, int ly, t_cub_manager *manager, int color)
{
	for (int i = 0;i < 360;++i)
	{
		cub_mlx_pixel_put(&manager->mlx_manager.img_data, x + radius * cos(radians(i)), y + radius * sin(radians(i)), lx, ly, color);
	}
}


// void drawMinimap(int map[][MAP_WIDTH], int playerX, int playerY) {
//     int minimap[MINIMAP_SIZE][MINIMAP_SIZE];
//     int minimapStartX = playerX - (MINIMAP_SIZE / 2);
//     int minimapStartY = playerY - (MINIMAP_SIZE / 2);
//     int minimapEndX = minimapStartX + MINIMAP_SIZE;
//     int minimapEndY = minimapStartY + MINIMAP_SIZE;

//     // Clear the minimap
//     for (int i = 0; i < MINIMAP_SIZE; i++) {
//         for (int j = 0; j < MINIMAP_SIZE; j++) {
//             minimap[i][j] = 0;
//         }
//     }

//     // Copy the relevant section of the map to the minimap
//     for (int i = minimapStartY; i < minimapEndY; i++) {
//         for (int j = minimapStartX; j < minimapEndX; j++) {
//             int minimapX = j - minimapStartX;
//             int minimapY = i - minimapStartY;
//             if (i >= 0 && i < MAP_HEIGHT && j >= 0 && j < MAP_WIDTH) {
//                 minimap[minimapY][minimapX] = map[i][j];
//             }
//         }
//     }

//     // Draw the minimap
//     for (int i = 0; i < MINIMAP_SIZE; i++) {
//         for (int j = 0; j < MINIMAP_SIZE; j++) {
//             if (minimap[i][j] == 1) {
//                 printf("#");
//             } else {
//                 printf(".");
//             }
//         }
//         printf("\n");
//     }
// }

int draw(t_cub_manager *manager)
{
	double	angle;
	int		num_of_rays;
	// int		i;
	// double	dist;
	// double	height;
	clear_window(manager, 0x00000000, WIDTH, HEIGHT);
	cast_all_rays(manager);
	draw_empty_rect(0, 0, 10 * mini_x, 6 * mini_y, WIDTH, HEIGHT, manager, 0x0000ff00);
	double mapsx = (manager->player.x - 5.0 * TILE_SIZE);
	// double mapex = manager->player.x + 5.0 * TILE_SIZE;
	double mapsy = manager->player.y - 3.0 * TILE_SIZE;
	// double mapey = manager->player.y + 3.0 * TILE_SIZE;
	for (int i = 0;i < mini_y * 6;++i)
	{
		for (int j = 0;j < mini_x * 10;++j)
		{
			int x = ((mapsy + i) * mini_x / TILE_SIZE);
			int y = ((mapsx + j) * mini_y / TILE_SIZE);
			if (x >= 0 && x < manager->map->map_height && y >= 0 && y < manager->map->map_width && manager->map->map[x][y] == '1')
				cub_mlx_pixel_put(&manager->mlx_manager.img_data, j, i, mini_x * 10, mini_y * 6, 0x0029af00);
		}
	}

	draw_line(round(mini_x * 10 / 2.0), round(mini_y * 6 / 2.0), mini_x * 10 / 2.0 + cos(manager->player.rotation_angle) * 10, mini_y * 6 / 2.0 + sin(manager->player.rotation_angle) * 10, mini_x * 10, mini_y * 6, manager, 0x00ff0000);
	// // draw_line(mini_x * 15 / 2.0, mini_y * 10 / 2.0, mini_x * 15 / 2.0 + cos(manager->player.rotation_angle) * 15, mini_y * 10 / 2.0 + sin(manager->player.rotation_angle) * 15, manager, 0x00ff0000);
	angle = manager->player.rotation_angle - (radians(FOV / 2.0));
	num_of_rays = WIDTH / (double) (WALL_STRIP_WIDTH);

	mlx_put_image_to_window(manager->mlx_manager.mlx, manager->mlx_manager.mlx_window, manager->mlx_manager.img_data.img, 0, 0);
	return 0;
}

double 	__distance(double x, double y, double x1, double y1)
{
	return (sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y)));
}

bool	__inside_wall(int x, int y, bool isfacingup, t_cub_manager* manager)
{
	int	x_index;
	int	y_index;

	if (isfacingup == true)
		y -= 1;
	x_index = x / TILE_SIZE;
	y_index = y / TILE_SIZE;
	if (!(x_index >= 0 && x_index < manager->map->map_width && y_index >= 0 && y_index < manager->map->map_height))
		return (false);
	return (manager->map->map[y_index][x_index] == '1');
}
bool	__inside_wall_ver(int x, int y, bool isfacingleft, t_cub_manager* manager)
{
	int	x_index;
	int	y_index;

	if (isfacingleft == true)
		x -= 1;
	x_index = x / TILE_SIZE;
	y_index = y / TILE_SIZE;
	if (!(x_index >= 0 && x_index < manager->map->map_width && y_index >= 0 && y_index < manager->map->map_height))
		return (false);
	return (manager->map->map[y_index][x_index] == '1');
}

void	cast(t_ray* ray, t_cub_manager* manager)
{
	double	xintercept;
	double	yintercept;
	double	xstep, ystep;

	bool found_horz_wall_hit = false;
	double	horz_wall_hit_x = 0;
	double	horz_wall_hit_y = 0;
	// double horz_wall_color = 0;

	//fid the y-coordinates of the closest horiz grid intersection
	yintercept = floor(manager->player.y / TILE_SIZE) * (double)TILE_SIZE;
	if (ray->isRayFacingDown)
		yintercept += TILE_SIZE;

	xintercept = manager->player.x + (yintercept - manager->player.y) / tan(ray->rayAngle);//?radians or degrees
	ystep = TILE_SIZE;
	if (ray->isRayFacingUp)
		ystep *= -1;

	xstep = TILE_SIZE / tan(ray->rayAngle);//radians or degrees
	if (ray->isRayFacingLeft && xstep > 0)
		xstep *= -1;
	if (ray->isRayFacingRight && xstep < 0)
		xstep *= -1;

	double next_horz_touch_x = xintercept;
	double next_horz_touch_y = yintercept;


    // Increment xstep and ystep until we find a wall
	while (next_horz_touch_x >= 0 && next_horz_touch_x < manager->map->map_width * TILE_SIZE && next_horz_touch_y >= 0 && next_horz_touch_y < manager->map->map_height * TILE_SIZE)
	{
		if (__inside_wall(next_horz_touch_x, next_horz_touch_y, ray->isRayFacingUp, manager))
		{
			found_horz_wall_hit = true;
			horz_wall_hit_x = next_horz_touch_x;
			horz_wall_hit_y = next_horz_touch_y;
			// horz_wall_color = content; //don't forget that
			// printf("found horizontal wall\n-----------------------------------\n\n");
			break ;
		}
		// printf("nexthx: %lf nexhy: %lf (%s:%d)\n-------------------\n", next_horz_touch_x, next_horz_touch_y, __FILE__, __LINE__);
		next_horz_touch_x += xstep;
		next_horz_touch_y += ystep;
	}

	//vertical ray-grid intersection code

	bool found_ver_hit = false;
	double ver_hit_x = 0;
	double ver_hit_y = 0;
	//don't forget the color

	xintercept = floor(manager->player.x / TILE_SIZE) * TILE_SIZE;
	if (ray->isRayFacingRight)
		xintercept += TILE_SIZE;
	yintercept = manager->player.y + (xintercept - manager->player.x) * tan(ray->rayAngle);

	xstep = TILE_SIZE;
	if (ray->isRayFacingLeft)
		xstep = -xstep;
	ystep = TILE_SIZE * tan(ray->rayAngle);
	if (ray->isRayFacingUp && ystep > 0)
		ystep *= -1;
	if (ray->isRayFacingDown && ystep < 0)
		ystep *= -1;

	double next_ver_touch_x = xintercept;
	double next_ver_touch_y = yintercept;

	while (next_ver_touch_x >= 0 && next_ver_touch_x <manager->map->map_width * TILE_SIZE && next_ver_touch_y >= 0 && next_ver_touch_y < manager->map->map_height * TILE_SIZE)
	{
		if (__inside_wall_ver(next_ver_touch_x, next_ver_touch_y, ray->isRayFacingLeft, manager))
		{
			found_ver_hit = true;
			ver_hit_x = next_ver_touch_x;
			ver_hit_y = next_ver_touch_y;
			//get the color here
			// printf("found vertical wall\n-----------------------------------\n");
			break ;
		}
		// printf("nextvx: %lf nexvy: %lf (%s:%d)\n-------------------\n", next_ver_touch_x, next_ver_touch_y, __FILE__, __LINE__);
		next_ver_touch_x += xstep;
		next_ver_touch_y += ystep;
	}


	//calculate both hor and vert distances and choose the smallest value

	double horz_hit_distance = 0.0;
	double vert_hit_distance = 0.0;
	// if (found_horz_wall_hit)
		horz_hit_distance = hypot(manager->player.x - horz_wall_hit_x, manager->player.y - horz_wall_hit_y);
	// else
	// 	horz_hit_distance = 3.4E+38;//double max value
	// if (found_ver_hit)
		vert_hit_distance = hypot(manager->player.x - ver_hit_x, manager->player.y - ver_hit_y);
	// else
	// 	vert_hit_distance = 3.4E+38;//double max value

	// printf("ver_dis: %f\thor_dis: %f       (renderer.c:461)\n", vert_hit_distance, horz_hit_distance);
	if (found_horz_wall_hit && found_ver_hit)
	{
		if (horz_hit_distance < vert_hit_distance)
		{
			ray->wallHitX = horz_wall_hit_x;
			ray->wallHitY = horz_wall_hit_y;
			ray->distance = horz_hit_distance;
		}
		else
		{
			ray->wallHitX = ver_hit_x;
			ray->wallHitY = ver_hit_y;
			ray->distance = vert_hit_distance;
			ray->wasHitVertical = true;
		}
		// printf("bjoj\n");
	}
	else if (found_horz_wall_hit)
	{
		ray->wallHitX = horz_wall_hit_x;
		ray->wallHitY = horz_wall_hit_y;
		ray->distance = horz_hit_distance;
		// printf("horz\n");
	}
	else if (found_ver_hit)
	{
		// printf("ver\n");
		ray->wallHitX = ver_hit_x;
		ray->wallHitY = ver_hit_y;
		ray->distance = vert_hit_distance;
		ray->wasHitVertical = true;
    }
}
	// printf("")
	// printf("x0: %f, y0: %f, x1: %f, y1: %f\n", manager->player.x,  manager->player.y, ray->wallHitX, ray->wallHitX);
	// draw_line(manager->player.x * SCALING_FACTOR, manager->player.y * SCALING_FACTOR, ray->wallHitX * SCALING_FACTOR, ray->wallHitY, 0, 0, manager, 0xff0000);

void	__initialize_ray_attributes(t_ray *ray)
{
	ray->distance = 0;
	ray->wallHitX = 0;
	ray->wallHitY = 0;
	ray->wasHitVertical = false;
	//color here
	if (ray->rayAngle > 0 && ray->rayAngle < M_PI)
		ray->isRayFacingDown = true;
	else
		ray->isRayFacingDown = false;
	ray->isRayFacingUp = !ray->isRayFacingDown;
	if (ray->rayAngle < M_PI_2 || ray->rayAngle > 1.5 * M_PI)
		ray->isRayFacingRight = true;
	else
		ray->isRayFacingRight = false;
	ray->isRayFacingLeft = !ray->isRayFacingRight;
}

void	render_3d_projected_walls(t_cub_manager* manager);
void	cast_all_rays(t_cub_manager* manager)
{
	double	ray_angle;
	int		i;
	double	angle_increment;

	// manager->rays = (t_ray *)malloc(NUMBER_OF_RAYS * sizeof(t_ray));//TODO allocate needed memory for each ray before calling this funcition
	angle_increment = radians(FOV) / NUMBER_OF_RAYS;
	ray_angle = manager->player.rotation_angle - (radians(FOV) / 2.0);
	i = -1;
	while (++i < NUMBER_OF_RAYS)
	{
		normalize_angle(&ray_angle);
		manager->rays[i].rayAngle =  ray_angle;
		__initialize_ray_attributes(&manager->rays[i]);
		cast(&manager->rays[i], manager);
		ray_angle+= angle_increment;
	}
	// render_3d_projected_walls(manager);
	rendering_3d_walls(manager);
}

// void	render_3d_projected_walls(t_cub_manager* manager)
// {
// 	t_ray	ray;
// 	int		i;

// 	i = -1;
// 	double disProj = (WIDTH / 2) / tan(radians(FOV) / 2);
// 	while (++i < NUMBER_OF_RAYS)
// 	{
// 		ray = manager->rays[i];
// 		double correctWallDistance = ray.distance * cos(ray.rayAngle - manager->player.rotation_angle);
// 		double wallSheight = (TILE_SIZE / correctWallDistance) * disProj;
// 		//colors

// 		draw_full_rect(i * WALL_STRIP_WIDTH, (HEIGHT / 2) - (wallSheight / 2), WALL_STRIP_WIDTH, wallSheight, WIDTH, HEIGHT, manager, 0xffffff); // lx and ly in this function are used to not go beyond the drawing area limits
// 	}
// }

void	a_select_the_right_texture(t_cub_manager* manager, int ray_index)
{

	if (manager->rays[ray_index].wasHitVertical && manager->rays[ray_index].isRayFacingRight)
		manager->mlx_manager.selected_texture = manager->mlx_manager.east_texture;
	if (manager->rays[ray_index].wasHitVertical && manager->rays[ray_index].isRayFacingLeft)
		manager->mlx_manager.selected_texture = manager->mlx_manager.west_texture;
	if (!manager->rays[ray_index].wasHitVertical && manager->rays[ray_index].isRayFacingUp)
		manager->mlx_manager.selected_texture = manager->mlx_manager.north_texture;
	manager->mlx_manager.selected_texture = manager->mlx_manager.south_texture;
}

int	__get_x_offset(t_cub_manager* manager, int ray_id)
{
	// (void) texture;
	double	offx;

	if (manager->rays[ray_id].wasHitVertical)
		offx = fmod(manager->rays[ray_id].wallHitY, TILE_SIZE);
	else
		offx = fmod(manager->rays[ray_id].wallHitX, TILE_SIZE);
	offx = offx / TILE_SIZE * (manager->mlx_manager.selected_texture.texture_width);
	return ((int) offx);
}

int	__get_y_offset(int wall_strip_height, int y)
{
	int	dis_from_top;

	dis_from_top = y + (wall_strip_height / 2) - (HEIGHT / 2);//TODO: CHECK THIS (brackets may not closed probably)
	return (dis_from_top * ((double)TILE_SIZE / wall_strip_height));
}

uint32_t	__get_pixel_color(t_cub_manager* manager, int ray_id, int wall_strip_height, int y)
{
	int		offset_x;
	int		offset_y;
	int			color;

	__select_the_right_texture(manager, ray_id);
	offset_x = __get_x_offset(manager, ray_id);
	offset_y = __get_y_offset(wall_strip_height, y);
	color = *(int *)((manager->mlx_manager.selected_texture.addr + (offset_y * manager->mlx_manager.selected_texture.texture_width + offset_x * (manager->mlx_manager.selected_texture.bits_per_pixel / 8))));
	return (color);	
}
// void	rendering_3d_walls(t_cub_manager* manager)
// {
// 	// clear_window(manager, 0x000000, WIDTH, HEIGHT);
		
// 	    for (int i = 0; i < NUMBER_OF_RAYS; i++) {
// 			// printf("distance: %f\n", manager->rays[i].distance);
// 		__select_the_right_texture(manager, i);
//         double perpDistance = manager->rays[i].distance * cos(manager->rays[i].rayAngle - manager->player.rotation_angle);
// 		// printf("%lf, %lf, (%s:%d)\n", manager->rays[i].distance, cos(manager->rays[i].rayAngle - manager->player.rotation_angle), __FILE__, __LINE__);
//         double distanceProjPlane = (WIDTH / 2.0) / tan(radians(FOV) / 2.0);
//         double projectedWallHeight = ((double)TILE_SIZE / perpDistance) * distanceProjPlane;

//         int wallStripHeight = (int)projectedWallHeight;

//         int wallTopPixel = (HEIGHT / 2.0) - (wallStripHeight / 2.0);
//         // wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

//         int wallBottomPixel = (HEIGHT / 2.0) + (wallStripHeight / 2.0);
//         // wallBottomPixel = wallBottomPixel > HEIGHT ? HEIGHT : wallBottomPixel;

// 		double offx;
// 		if (manager->rays[i].wasHitVertical)
// 		{
// 			offx = fmod(manager->rays[i].wallHitY, TILE_SIZE);
// 		}
// 		else
// 			offx = fmod(manager->rays[i].wallHitX, TILE_SIZE);
// 		offx = offx / TILE_SIZE * manager->mlx_manager.selected_texture.texture_width;
//         // render the wall from wallTopPixel to wallBottomPixel
//         for (int j = wallTopPixel; j < wallBottomPixel;++j)
// 		{
// 			// double offy = (j - wallTopPixel) * (manager->mlx_manager.selected_texture.texture_height / (double)wallStripHeight);
// 			// printf("addr: %p\n", t->addr);
// 			cub_mlx_pixel_put(&manager->mlx_manager.img_data, i * WALL_STRIP_WIDTH, j, WIDTH, HEIGHT, __get_pixel_color(manager, i, wallStripHeight, j));
// 		}
//     }

// }
void	rendering_3d_walls(t_cub_manager* manager)
{
	// clear_window(manager, 0x000000, WIDTH, HEIGHT);
		int wi, hi, bbp, line, endian;
		void *img = mlx_xpm_file_to_image(manager->mlx_manager.mlx, "/Users/osallak/Desktop/normed-version-cub3d/assets/t.xpm", &wi, &hi);
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
void	__load_textures(t_cub_manager* manager)
{
	//decoding xpm files

	manager->map->wall_textures[NORTH].img = mlx_xpm_file_to_image(manager->mlx_manager.mlx, manager->map->no, &manager->map->wall_textures[NORTH].wi, &manager->map->wall_textures[NORTH].hi);
	manager->map->wall_textures[SOUTH].img = mlx_xpm_file_to_image(manager->mlx_manager.mlx, manager->map->so, &manager->map->wall_textures[SOUTH].wi, &manager->map->wall_textures[SOUTH].hi);
	manager->map->wall_textures[EAST].img = mlx_xpm_file_to_image(manager->mlx_manager.mlx, manager->map->ea, &manager->map->wall_textures[EAST].wi, &manager->map->wall_textures[EAST].hi);
	manager->map->wall_textures[WEST].img = mlx_xpm_file_to_image(manager->mlx_manager.mlx, manager->map->we, &manager->map->wall_textures[WEST].wi, &manager->map->wall_textures[WEST].hi);

	//protect
	if (!manager->map->wall_textures[NORTH].img || !manager->map->wall_textures[SOUTH].img || !manager->map->wall_textures[EAST].img || !manager->map->wall_textures[WEST].img)
	 panic("Error while loading textures");
	//initializing img data attributes

	puts("initializing img data attributes");
	manager->map->wall_textures[NORTH].tex_img_data.addr = mlx_get_data_addr(manager->map->wall_textures[NORTH].img, &manager->map->wall_textures[NORTH].tex_img_data.bits_per_pixel, &manager->map->wall_textures[NORTH].tex_img_data.line_length, &manager->map->wall_textures[NORTH].tex_img_data.endian);
	manager->map->wall_textures[SOUTH].tex_img_data.addr = mlx_get_data_addr(manager->map->wall_textures[SOUTH].img, &manager->map->wall_textures[SOUTH].tex_img_data.bits_per_pixel, &manager->map->wall_textures[SOUTH].tex_img_data.line_length, &manager->map->wall_textures[SOUTH].tex_img_data.endian);
	manager->map->wall_textures[EAST].tex_img_data.addr = mlx_get_data_addr(manager->map->wall_textures[EAST].img, &manager->map->wall_textures[EAST].tex_img_data.bits_per_pixel, &manager->map->wall_textures[EAST].tex_img_data.line_length, &manager->map->wall_textures[EAST].tex_img_data.endian);
	if (!manager->map->wall_textures[NORTH].tex_img_data.addr || !manager->map->wall_textures[SOUTH].tex_img_data.addr || !manager->map->wall_textures[EAST].tex_img_data.addr || !manager->map->wall_textures[WEST].tex_img_data.addr)
		panic("error: coudn't load textures");
	puts("after initializing img data attributes");
}

int render(t_map_manager *map_manager)
{
	int	i;
	int	j;
	int	found_player;
	t_cub_manager manager;

	manager.mlx_manager.mlx = mlx_init();
	manager.map = map_manager;
	manager.map->map_width = get_map_width(manager.map);
	manager.map->map_height = get_map_height(manager.map);
	char **map = malloc(sizeof(char *) * manager.map->map_height);
	for (int i = 0; i < manager.map->map_height; ++i)
	{
		map[i] = malloc(manager.map->map_width);
	}
	for (int i = 0;i < manager.map->map_height; ++i)
	{
		for (int j = 0;j < manager.map->map_width;++j)
		{
			if (j < (int) ft_strlen(manager.map->map[i]))
				map[i][j] = manager.map->map[i][j];
			else
				map[i][j] = ' ';
		}
	}
	manager.map->map = map;
	found_player = 0;
	i = -1;
	manager.player.rotation_speed = radians(R_SPEED);
	manager.player.walk_speed = 10;
	while (++i < manager.map->map_height)
	{
		j = -1;
		while (++j < (int) ft_strlen(manager.map->map[i]))
		{
			if (ft_strchr(PLAYER_CHAR, manager.map->map[i][j]))
			{
				manager.player.x = j * TILE_SIZE + TILE_SIZE / 2.0;
				manager.player.y = i * TILE_SIZE + TILE_SIZE / 2.0;
				if (manager.map->map[i][j] == 'S')
					manager.player.rotation_angle = radians(90);
				if (manager.map->map[i][j] == 'N')
					manager.player.rotation_angle = radians(270);
				if (manager.map->map[i][j] == 'W')
					manager.player.rotation_angle = radians(180);
				if (manager.map->map[i][j] == 'E')
					manager.player.rotation_angle = 0;
				found_player = true;
				break;
			}
		}
		if (found_player)
			break;
	}
	manager.mlx_manager.mlx_window = mlx_new_window(manager.mlx_manager.mlx, WIDTH, HEIGHT, "cub3D");
	puts("before loading textures");
	__load_textures(&manager);
	manager.mlx_manager.img_data.img = mlx_new_image(manager.mlx_manager.mlx, WIDTH, HEIGHT);
	manager.mlx_manager.img_data.addr = mlx_get_data_addr(manager.mlx_manager.img_data.img, &manager.mlx_manager.img_data.bits_per_pixel, &manager.mlx_manager.img_data.line_length, &manager.mlx_manager.img_data.endian);
	manager.rays = malloc(NUMBER_OF_RAYS * sizeof(t_ray));
	draw(&manager);
	mlx_hook(manager.mlx_manager.mlx_window, ON_KEYDOWN, 1L<<0, controls, &manager);
	mlx_loop(manager.mlx_manager.mlx);
	return (0);
}