#include "raylib.h"

struct AnimData {
  Rectangle rec;
  Vector2 pos;
  int frame;
  float update_time;
  float running_time;
};

bool isOnGround(const AnimData &data, int window_height) {
  return data.pos.y >= window_height - data.rec.height;
}

void updateAnimData(AnimData *data, float dT, int max_frame) {
  data->running_time += dT;

  if (data->running_time >= data->update_time) {
    data->rec.x = data->frame * data->rec.width;
    data->frame++;
    if (data->frame > max_frame) {
      data->frame = 0;
    }

    data->running_time = 0.0;
  }
}

int main() {
  // Window vars
  int window_dimensions[2];
  window_dimensions[0] = 1080;
  window_dimensions[1] = 720;

  constexpr auto WD_TITLE{"Whipshout's test"};

  // Rectangle movement
  int velocity{0};
  bool can_jump{true};
  constexpr int JUMP_VELOCITY{-600};

  // World vars
  constexpr int gravity{1000};

  bool collision{false};

  // Raylib init
  InitWindow(window_dimensions[0], window_dimensions[1], WD_TITLE);
  SetTargetFPS(60);

  // Background
  Texture2D background{LoadTexture("./textures/far-buildings.png")};
  float bg_x{0.0};

  Texture2D midground{LoadTexture("./textures/back-buildings.png")};
  float mg_x{0.0};

  Texture2D foreground{LoadTexture("./textures/foreground.png")};
  float fg_x{0.0};

  // Scarfy char
  Texture2D scarfy{LoadTexture("./textures/scarfy.png")};

  AnimData scarfy_data;
  scarfy_data.rec.width = scarfy.width / 6;
  scarfy_data.rec.height = scarfy.height;
  scarfy_data.rec.x = 0;
  scarfy_data.rec.y = 0;
  scarfy_data.pos.x = window_dimensions[0] / 2 - scarfy_data.rec.width / 2;
  scarfy_data.pos.y = window_dimensions[1] - scarfy_data.rec.height;
  scarfy_data.frame = 0;
  scarfy_data.update_time = 1.0 / 12.0;
  scarfy_data.running_time = 0.0;

  // Nebula
  Texture2D nebula{LoadTexture("./textures/12_nebula_spritesheet.png")};

  AnimData nebulae[3]{};
  constexpr int nebulae_length = sizeof(nebulae) / sizeof(nebulae[0]);

  for (int i = 0; i < nebulae_length; ++i) {
    nebulae[i].rec.x = 0.0;
    nebulae[i].rec.y = 0.0;
    nebulae[i].rec.width = nebula.width / 8;
    nebulae[i].rec.height = nebula.height / 8;
    nebulae[i].pos.x = window_dimensions[0] + i * 300;
    nebulae[i].pos.y = window_dimensions[1] - nebula.height / 8;
    nebulae[i].frame = 0;
    nebulae[i].update_time = 1.0 / 16.0;
    nebulae[i].running_time = 0.0;
  }

  float finish_lane{nebulae[nebulae_length - 1].pos.x + 200};

  int nebula_velocity{-220};

  // Raylib window renders
  while (!WindowShouldClose()) {
    const float dT{GetFrameTime()};

    BeginDrawing();
    ClearBackground(WHITE);

    bg_x -= 100 * dT;
    if (bg_x <= -background.width * 5) {
      bg_x = 0.0;
    }

    mg_x -= 200 * dT;
    if (mg_x <= -midground.width * 5) {
      mg_x = 0.0;
    }

    fg_x -= 400 * dT;
    if (fg_x <= -foreground.width * 5) {
      fg_x = 0.0;
    }

    // Backgrounds
    Vector2 bg1_pos{bg_x, 0.0};
    DrawTextureEx(background, bg1_pos, 0.0, 5.0, WHITE);

    Vector2 bg2_pos{bg_x + background.width * 5, 0.0};
    DrawTextureEx(background, bg2_pos, 0.0, 5.0, WHITE);

    Vector2 mg1_pos{mg_x, 0.0};
    DrawTextureEx(midground, mg1_pos, 0.0, 5.0, WHITE);

    Vector2 mg2_pos{mg_x + midground.width * 5, 0.0};
    DrawTextureEx(midground, mg2_pos, 0.0, 5.0, WHITE);

    Vector2 fg1_pos{fg_x, 0.0};
    DrawTextureEx(foreground, fg1_pos, 0.0, 5.0, WHITE);

    Vector2 fg2_pos{fg_x + foreground.width * 5, 0.0};
    DrawTextureEx(foreground, fg2_pos, 0.0, 5.0, WHITE);

    // Ground check
    if (isOnGround(scarfy_data, window_dimensions[1])) {
      velocity = 0;
      can_jump = true;
    } else {
      velocity += gravity * dT;
      can_jump = false;
    }

    // Jump input
    if (IsKeyPressed(KEY_SPACE) && can_jump) {
      velocity += JUMP_VELOCITY;
    }

    // Scarfy movement
    scarfy_data.pos.y += velocity * dT;

    // Nebula movement + animation
    for (auto &i : nebulae) {
      i.pos.x += nebula_velocity * dT;

      updateAnimData(&i, dT, 7);
    }

    // Update finish lane
    finish_lane += nebula_velocity * dT;

    // Update scarfy animation
    if (can_jump) {
      updateAnimData(&scarfy_data, dT, 5);
    }

    for (auto &i : nebulae) {
      float pad{50};
      Rectangle rc_nebula{
          i.pos.x + pad,
          i.pos.y + pad,
          i.rec.width - 2 * pad,
          i.rec.height - 2 * pad,
      };

      Rectangle rc_scarfy{
          scarfy_data.pos.x,
          scarfy_data.pos.y,
          scarfy_data.rec.width,
          scarfy_data.rec.height,
      };

      if (CheckCollisionRecs(rc_nebula, rc_scarfy)) {
        collision = true;
      }
    }

    // Draw textures
    if (collision) {
      DrawText("Game Over!", window_dimensions[0] / 2 - 170,
               window_dimensions[1] / 2 - 50, 80, WHITE);
    } else if (scarfy_data.pos.x >= finish_lane) {
      DrawText("You Win!", window_dimensions[0] / 2 - 150,
               window_dimensions[1] / 2 - 50, 80, WHITE);
    } else {
      for (auto &i : nebulae) {
        DrawTextureRec(nebula, i.rec, i.pos, WHITE);
      }
      DrawTextureRec(scarfy, scarfy_data.rec, scarfy_data.pos, WHITE);
    }

    EndDrawing();
  }

  UnloadTexture(scarfy);
  UnloadTexture(nebula);
  UnloadTexture(background);
  UnloadTexture(midground);
  UnloadTexture(foreground);

  CloseWindow();

  return 0;
}