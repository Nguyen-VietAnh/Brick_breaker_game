#include "Ball.h"

Ball::Ball()
{
	ball_texture = NULL;
	x = 0;
	y = 0;
	speed_x = 0;
	speed_y = 0;
	button_event = true;
	paddle_collision = false;
	brick_collision = false;
}

Ball::~Ball()
{
	SDL_DestroyTexture(ball_texture);
	ball_texture = NULL;
}

Ball& Ball::operator=(const Ball& other)
{
	this->ball_texture = other.ball_texture;
	this->x = other.x;
	this->y = other.y;
	this->speed_x = other.speed_x;
	this->speed_y = other.speed_y;
	this->paddle_collision = other.paddle_collision;
	this->brick_collision = other.brick_collision;
    
    return *this;
}

void Ball::set_start_position(const int& paddle_x, const int& paddle_y, const int& paddle_width, const int& paddle_height)
{
	x = paddle_x + (paddle_width - BALL_WIDTH) / 2.0;
	y = PLAY_GROUND_HEIGHT + (SCREEN_HEIGHT - PLAY_GROUND_HEIGHT) / 2.0 - paddle_height - BALL_HEIGHT - 5;
}

void Ball::load_ball_texture(SDL_Renderer* renderer)
{
	load_texture(renderer, BALL_IMAGE_FILE.c_str(), ball_texture);
}

void Ball::render_ball(SDL_Renderer* renderer)
{
	int x_ = static_cast<int>(x);
	int y_ = static_cast<int>(y);
	SDL_Rect renderQuad = { x_, y_, BALL_WIDTH, BALL_HEIGHT };
	SDL_RenderCopy(renderer, ball_texture, NULL, &renderQuad);
}

void Ball::set_ball(SDL_Event &events, bool &is_set_start_pos)
{
	if (button_event)
	{
		if (events.type == SDL_MOUSEBUTTONDOWN)
		{
			if (events.button.button == SDL_BUTTON_LEFT)
			{
				speed_x = -BALL_SPEED;
				speed_y = -BALL_SPEED;
				button_event = false;
				is_set_start_pos = false;
			}
		}
	}
}

void Ball::move(bool &is_set_start_pos, bool& lose_lives_check, Mix_Chunk* ball_impact_sound, bool& is_activated)
{
	x += speed_x;
	y += speed_y;
	if (is_activated)
	{
		x += speed_x;
		y += speed_y;
	}
	if (button_event)
	{
		x -= speed_x;
		y -= speed_y;
	}

	if (x < (SCREEN_WIDTH - PLAY_GROUND_WIDTH) / 2.0)
	{
		speed_x = fabs(speed_x);
		Mix_PlayChannel(-1, ball_impact_sound, 0);
	}
	if (x > PLAY_GROUND_WIDTH + (SCREEN_WIDTH - PLAY_GROUND_WIDTH) / 2.0 - BALL_WIDTH)
	{
		speed_x = -fabs(speed_x);
		Mix_PlayChannel(-1, ball_impact_sound, 0);
	}
	if (y < (SCREEN_HEIGHT - PLAY_GROUND_HEIGHT) / 2.0)
	{
		speed_y = fabs(speed_y);
		Mix_PlayChannel(-1, ball_impact_sound, 0);
	}
	if (y > PLAY_GROUND_HEIGHT + (SCREEN_HEIGHT - PLAY_GROUND_HEIGHT) / 2.0 - BALL_HEIGHT)
	{
		is_set_start_pos = true;
		button_event = true;
		lose_lives_check = true;
	}
	if (lose_lives_check) is_activated = false;
}

void Ball::event_move(Mix_Chunk* ball_impact_sound, bool& is_deleted, const bool& is_activated)
{
	x += speed_x;
	y += speed_y;
	if (is_activated)
	{
		x += speed_x;
		y += speed_y;
	}

	if (x < (SCREEN_WIDTH - PLAY_GROUND_WIDTH) / 2.0)
	{
		speed_x = fabs(speed_x);
		Mix_PlayChannel(-1, ball_impact_sound, 0);
	}
	if (x > PLAY_GROUND_WIDTH + (SCREEN_WIDTH - PLAY_GROUND_WIDTH) / 2.0 - BALL_WIDTH)
	{
		speed_x = -fabs(speed_x);
		Mix_PlayChannel(-1, ball_impact_sound, 0);
	}
	if (y < (SCREEN_HEIGHT - PLAY_GROUND_HEIGHT) / 2.0)
	{
		speed_y = fabs(speed_y);
		Mix_PlayChannel(-1, ball_impact_sound, 0);
	}
	if (y > PLAY_GROUND_HEIGHT + (SCREEN_HEIGHT - PLAY_GROUND_HEIGHT) / 2.0 - BALL_HEIGHT)
	{
		is_deleted = true;
	}
}

void Ball::handle_paddle_collision(const int& paddle_x, const int& paddle_y, const int& paddle_width, const int& paddle_height,
								const bool& is_set_start_pos, Mix_Chunk* ball_impact_sound)
{
	paddle_collision = false;
	if ((x + BALL_WIDTH / 2.0) >= paddle_x && (x + BALL_WIDTH / 2.0) <= (paddle_x + paddle_width) && (y + BALL_HEIGHT) >= paddle_y)
	{
		paddle_collision = true;
		speed_y = -fabs(speed_y);
		if (!is_set_start_pos)
		{
			Mix_PlayChannel(-1, ball_impact_sound, 0);
		}
	}
}

void Ball::handle_brick_collision(const int& brick_x, const int& brick_y, const int& brick_width, const int& brick_height,
								double& brick_matrix, int& score, Mix_Chunk* ball_impact_sound, int& destroyed_bricks)
{
	int x_ = floor(x) - 1;
	int y_ = floor(y) - 1;
	SDL_Rect ball_rect = { x_, y_, BALL_WIDTH, BALL_HEIGHT };
	SDL_Rect brick_rect = { brick_x, brick_y, brick_width, brick_height };
	brick_collision = false;
	if (SDL_HasIntersection(&ball_rect, &brick_rect))
	{
		brick_collision = true;
		
		Mix_PlayChannel(-1, ball_impact_sound, 0);
		if (brick_matrix < 4.0)
		{
			brick_matrix --;
			score += 50;
		}
		if (brick_matrix == 0)
		{
			destroyed_bricks++;
		}

		double d1 = fabs(y + BALL_HEIGHT - brick_y);
		double d2 = fabs(y - (brick_y + brick_height));
		double d3 = fabs(x + BALL_WIDTH - brick_x);
		double d4 = fabs(x - (brick_x + brick_width));
		double min = d1;
		if (d2 <= min) min = d2;
		if (d3 <= min) min = d3;
		if (d4 <= min) min = d4;

		if (min == d1)
		{
			speed_y = -fabs(speed_y);
		}
		else if (min == d2)
		{
			speed_y = fabs(speed_y);
		}
		else if (min == d3)
		{
			speed_x = -fabs(speed_x);
		}
		else if (min == d4)
		{
			speed_x = fabs(speed_x);
		}
	}
}

void Ball::free()
{
	SDL_DestroyTexture(ball_texture);
	ball_texture = NULL;
	x = 0;
	y = 0;
	speed_x = 0;
	speed_y = 0;

	paddle_collision = false;
	brick_collision = false;
}