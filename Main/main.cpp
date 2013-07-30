////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2009 Alexander Poluektov
// All rights reserved
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// $Id: //depot/main/Main/main.cpp#11 $
// $DateTime: 2009/08/14 14:49:13 $

// Entry point of application.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine/Logging/Logging.h"
#include "Engine/Window/Window.h"
#include "Engine/Rendering/Direct3D/Direct3D_renderer.h"
#include "Engine/Input/Win32_input_handler.h"

#include <string>

#include <direct.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Engine
{
namespace Logging
{

const uchar sender_main = 1;

#define LOG_MAIN(message_level) LOG(Engine::Logging::sender_main, message_level)

}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Input;
using namespace Engine::Logging;
namespace Logging = Engine::Logging;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_curdir_to_appdir();
void init_colored_sprites(Colored_sprite* sprites);
void init_textured_sprites(Textured_sprite* tex_sprites);
void init_multitextured_sprites(Multitextured_2_sprite* tex2_sprites);
void handle_some_input(Input_handler&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int /*argc*/, char** /*argv*/)
{
   try
   {
      Logger::init(0, 0);
      Logger::set_global_message_level(Logging::minor);

      set_curdir_to_appdir();

      Window window("app.name", 100, 100, 300, 300);
      Direct3D_renderer renderer(window.get_handle(), true);
      Win32_input_handler input_handler(window);

      Colored_sprite sprites[2];
      init_colored_sprites(sprites);

      Textured_sprite tex_sprites[2];
      init_textured_sprites(tex_sprites);

      Multitextured_2_sprite tex2_sprites[1];
      init_multitextured_sprites(tex2_sprites);

      while (!window.is_closing())
      {
         window.handle_messages();

         if (!renderer.is_focused())
         {
            renderer.try_restore();
            continue;
         }

         handle_some_input(input_handler);

         renderer.add_to_scene(sprites[0]);
         renderer.add_to_scene(sprites[1]);
         renderer.add_to_scene(tex_sprites[0]);
         renderer.add_to_scene(tex_sprites[1]);
         renderer.add_to_scene(tex2_sprites[0]);
         renderer.add_to_scene(sprites[0]);
         renderer.add_to_scene(sprites[1]);
         renderer.add_to_scene(tex_sprites[0]);
         renderer.add_to_scene(tex_sprites[1]);
         renderer.add_to_scene(tex2_sprites[0]);

         renderer.render_scene();
         renderer.clear_scene();
      }

      LOG_MAIN(Logging::major) << "Exit from main succesfully";
      return 0;
   }
   catch (const std::runtime_error& ex)
   {
      LOG_MAIN(Logging::critical) << "!!! Exception catched !!!";
      LOG_MAIN(Logging::critical) << "!!!   Type:    " << typeid(ex).name() << " !!!";
      LOG_MAIN(Logging::critical) << "!!!   Message: " << ex.what() << " !!!";
      LOG_MAIN(Logging::critical) << "!!! Exit from main ubnormally; return code 1 !!!";
      return 1;
   }
   catch (...)
   {
      LOG_MAIN(Logging::critical) << "!!! Unknown exception catched !!!";
      LOG_MAIN(Logging::critical) << "!!! Exit from main ubnormally; return code 2 !!!";
      return 2;
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: separate module
// TODO: win32 specific, point it to developer
// TODO: test it
// TODO: split onto 3 functions (get file name, get directory name from file name, change directory)
void set_curdir_to_appdir()
{
   char file_name_c[MAX_PATH];
   DWORD n = ::GetModuleFileNameA(0, file_name_c, MAX_PATH);
   if (n == 0 || n == MAX_PATH)
   {
      // TODO: Win32_exception, please
      LOG_MAIN(Logging::critical) << "set_curdir_to_appdir() failed: 1";
      throw std::runtime_error("set_curdir_to_appdir() failed");
   }
   std::string file_name(file_name_c);
   LOG_MAIN(Logging::trivial) << "File name is " << file_name;

   size_t last_slash = file_name.find_last_of("\\/");
   std::string dir_name = "";
   if (last_slash == std::string::npos)
   {
      dir_name = "";
   }
   else
   {
      dir_name = file_name.substr(0, last_slash);
   }
   LOG_MAIN(Logging::trivial) << "Directory name is " << dir_name;

   if (chdir(dir_name.c_str()) == -1)
   {
      // TODO: Win32_exception, please
      LOG_MAIN(Logging::critical) << "set_curdir_to_appdir() failed: 2";
      throw std::runtime_error("set_curdir_to_appdir() failed");
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_colored_sprites(Colored_sprite* sprites)
{
   sprites[0].vertexes[0].position.x = 10;
   sprites[0].vertexes[0].position.y = 10;
   sprites[0].vertexes[0].position.z = 0.5f;
   sprites[0].vertexes[0].color.a = 0xff;
   sprites[0].vertexes[0].color.r = 0x7f;
   sprites[0].vertexes[0].color.g = 0x00;
   sprites[0].vertexes[0].color.b = 0x00;

   sprites[0].vertexes[1].position.x = 10;
   sprites[0].vertexes[1].position.y = 90;
   sprites[0].vertexes[1].position.z = 0.5f;
   sprites[0].vertexes[1].color.a = 0xff;
   sprites[0].vertexes[1].color.r = 0xff;
   sprites[0].vertexes[1].color.g = 0x00;
   sprites[0].vertexes[1].color.b = 0x00;

   sprites[0].vertexes[2].position.x = 90;
   sprites[0].vertexes[2].position.y = 90;
   sprites[0].vertexes[2].position.z = 0.5f;
   sprites[0].vertexes[2].color.a = 0xff;
   sprites[0].vertexes[2].color.r = 0xff;
   sprites[0].vertexes[2].color.g = 0x00;
   sprites[0].vertexes[2].color.b = 0x00;

   sprites[0].vertexes[3].position.x = 90;
   sprites[0].vertexes[3].position.y = 10;
   sprites[0].vertexes[3].position.z = 0.5f;
   sprites[0].vertexes[3].color.a = 0xff;
   sprites[0].vertexes[3].color.r = 0xff;
   sprites[0].vertexes[3].color.g = 0x00;
   sprites[0].vertexes[3].color.b = 0x00;

   sprites[1].vertexes[0].position.x = 10 + 200;
   sprites[1].vertexes[0].position.y = 10 + 200;
   sprites[1].vertexes[0].position.z = 0.5f;
   sprites[1].vertexes[0].color.a = 0xff;
   sprites[1].vertexes[0].color.r = 0x7f;
   sprites[1].vertexes[0].color.g = 0x00;
   sprites[1].vertexes[0].color.b = 0x00;

   sprites[1].vertexes[1].position.x = 10 + 200;
   sprites[1].vertexes[1].position.y = 90 + 200;
   sprites[1].vertexes[1].position.z = 0.5f;
   sprites[1].vertexes[1].color.a = 0xff;
   sprites[1].vertexes[1].color.r = 0xff;
   sprites[1].vertexes[1].color.g = 0x00;
   sprites[1].vertexes[1].color.b = 0x00;

   sprites[1].vertexes[2].position.x = 90 + 200;
   sprites[1].vertexes[2].position.y = 90 + 200;
   sprites[1].vertexes[2].position.z = 0.5f;
   sprites[1].vertexes[2].color.a = 0xff;
   sprites[1].vertexes[2].color.r = 0xff;
   sprites[1].vertexes[2].color.g = 0x00;
   sprites[1].vertexes[2].color.b = 0x00;

   sprites[1].vertexes[3].position.x = 90 + 200;
   sprites[1].vertexes[3].position.y = 10 + 200;
   sprites[1].vertexes[3].position.z = 0.5f;
   sprites[1].vertexes[3].color.a = 0xff;
   sprites[1].vertexes[3].color.r = 0xff;
   sprites[1].vertexes[3].color.g = 0x00;
   sprites[1].vertexes[3].color.b = 0x00;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_textured_sprites(Textured_sprite* tex_sprites)
{
   tex_sprites[0].vertexes[0].position.x = 110;
   tex_sprites[0].vertexes[0].position.y = 110;
   tex_sprites[0].vertexes[0].position.z = 0.5f;
   tex_sprites[0].vertexes[0].color.a = 0xff;
   tex_sprites[0].vertexes[0].color.r = 0x7f;
   tex_sprites[0].vertexes[0].color.g = 0x00;
   tex_sprites[0].vertexes[0].color.b = 0x00;
   tex_sprites[0].vertexes[0].texture_coord.tu = 0;
   tex_sprites[0].vertexes[0].texture_coord.tv = 0;

   tex_sprites[0].vertexes[1].position.x = 110;
   tex_sprites[0].vertexes[1].position.y = 190;
   tex_sprites[0].vertexes[1].position.z = 0.5f;
   tex_sprites[0].vertexes[1].color.a = 0xff;
   tex_sprites[0].vertexes[1].color.r = 0xff;
   tex_sprites[0].vertexes[1].color.g = 0x00;
   tex_sprites[0].vertexes[1].color.b = 0x00;
   tex_sprites[0].vertexes[1].texture_coord.tu = 0;
   tex_sprites[0].vertexes[1].texture_coord.tv = 1;

   tex_sprites[0].vertexes[2].position.x = 190;
   tex_sprites[0].vertexes[2].position.y = 190;
   tex_sprites[0].vertexes[2].position.z = 0.5f;
   tex_sprites[0].vertexes[2].color.a = 0xff;
   tex_sprites[0].vertexes[2].color.r = 0xff;
   tex_sprites[0].vertexes[2].color.g = 0x00;
   tex_sprites[0].vertexes[2].color.b = 0x00;
   tex_sprites[0].vertexes[2].texture_coord.tu = 1;
   tex_sprites[0].vertexes[2].texture_coord.tv = 1;

   tex_sprites[0].vertexes[3].position.x = 190;
   tex_sprites[0].vertexes[3].position.y = 110;
   tex_sprites[0].vertexes[3].position.z = 0.5f;
   tex_sprites[0].vertexes[3].color.a = 0xff;
   tex_sprites[0].vertexes[3].color.r = 0xff;
   tex_sprites[0].vertexes[3].color.g = 0x00;
   tex_sprites[0].vertexes[3].color.b = 0x00;
   tex_sprites[0].vertexes[3].texture_coord.tu = 1;
   tex_sprites[0].vertexes[3].texture_coord.tv = 0;

   tex_sprites[0].blending = blending_mode_add;
   tex_sprites[0].texture.file_name = "banana.bmp";

   tex_sprites[1].vertexes[0].position.x = 10 + 200;
   tex_sprites[1].vertexes[0].position.y = 10;
   tex_sprites[1].vertexes[0].position.z = 0.5f;
   tex_sprites[1].vertexes[0].color.a = 0xff;
   tex_sprites[1].vertexes[0].color.r = 0x7f;
   tex_sprites[1].vertexes[0].color.g = 0x00;
   tex_sprites[1].vertexes[0].color.b = 0x00;
   tex_sprites[1].vertexes[0].texture_coord.tu = 0;
   tex_sprites[1].vertexes[0].texture_coord.tv = 0;

   tex_sprites[1].vertexes[1].position.x = 10 + 200;
   tex_sprites[1].vertexes[1].position.y = 90;
   tex_sprites[1].vertexes[1].position.z = 0.5f;
   tex_sprites[1].vertexes[1].color.a = 0xff;
   tex_sprites[1].vertexes[1].color.r = 0xff;
   tex_sprites[1].vertexes[1].color.g = 0x00;
   tex_sprites[1].vertexes[1].color.b = 0x00;
   tex_sprites[1].vertexes[1].texture_coord.tu = 0;
   tex_sprites[1].vertexes[1].texture_coord.tv = 1;

   tex_sprites[1].vertexes[2].position.x = 90 + 200;
   tex_sprites[1].vertexes[2].position.y = 90;
   tex_sprites[1].vertexes[2].position.z = 0.5f;
   tex_sprites[1].vertexes[2].color.a = 0xff;
   tex_sprites[1].vertexes[2].color.r = 0xff;
   tex_sprites[1].vertexes[2].color.g = 0x00;
   tex_sprites[1].vertexes[2].color.b = 0x00;
   tex_sprites[1].vertexes[2].texture_coord.tu = 1;
   tex_sprites[1].vertexes[2].texture_coord.tv = 1;

   tex_sprites[1].vertexes[3].position.x = 90 + 200;
   tex_sprites[1].vertexes[3].position.y = 10;
   tex_sprites[1].vertexes[3].position.z = 0.5f;
   tex_sprites[1].vertexes[3].color.a = 0xff;
   tex_sprites[1].vertexes[3].color.r = 0xff;
   tex_sprites[1].vertexes[3].color.g = 0x00;
   tex_sprites[1].vertexes[3].color.b = 0x00;
   tex_sprites[1].vertexes[3].texture_coord.tu = 1;
   tex_sprites[1].vertexes[3].texture_coord.tv = 0;

   tex_sprites[1].blending = blending_mode_modulate;
   tex_sprites[1].texture.file_name = "banana.bmp";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_multitextured_sprites(Multitextured_2_sprite* tex2_sprites)
{
   tex2_sprites[0].vertexes[0].position.x = 10;
   tex2_sprites[0].vertexes[0].position.y = 110;
   tex2_sprites[0].vertexes[0].position.z = 0.5f;
   tex2_sprites[0].vertexes[0].color.a = 0xff;
   tex2_sprites[0].vertexes[0].color.r = 0x7f;
   tex2_sprites[0].vertexes[0].color.g = 0x00;
   tex2_sprites[0].vertexes[0].color.b = 0x00;
   tex2_sprites[0].vertexes[0].texture_coord0.tu = 0;
   tex2_sprites[0].vertexes[0].texture_coord0.tv = 0;
   tex2_sprites[0].vertexes[0].texture_coord1.tu = 0;
   tex2_sprites[0].vertexes[0].texture_coord1.tv = 0;

   tex2_sprites[0].vertexes[1].position.x = 10;
   tex2_sprites[0].vertexes[1].position.y = 190;
   tex2_sprites[0].vertexes[1].position.z = 0.5f;
   tex2_sprites[0].vertexes[1].color.a = 0xff;
   tex2_sprites[0].vertexes[1].color.r = 0xff;
   tex2_sprites[0].vertexes[1].color.g = 0x00;
   tex2_sprites[0].vertexes[1].color.b = 0x00;
   tex2_sprites[0].vertexes[1].texture_coord0.tu = 0;
   tex2_sprites[0].vertexes[1].texture_coord0.tv = 1;
   tex2_sprites[0].vertexes[1].texture_coord1.tu = 0;
   tex2_sprites[0].vertexes[1].texture_coord1.tv = 1;

   tex2_sprites[0].vertexes[2].position.x = 90;
   tex2_sprites[0].vertexes[2].position.y = 190;
   tex2_sprites[0].vertexes[2].position.z = 0.5f;
   tex2_sprites[0].vertexes[2].color.a = 0xff;
   tex2_sprites[0].vertexes[2].color.r = 0xff;
   tex2_sprites[0].vertexes[2].color.g = 0x00;
   tex2_sprites[0].vertexes[2].color.b = 0x00;
   tex2_sprites[0].vertexes[2].texture_coord0.tu = 1;
   tex2_sprites[0].vertexes[2].texture_coord0.tv = 1;
   tex2_sprites[0].vertexes[2].texture_coord1.tu = 1;
   tex2_sprites[0].vertexes[2].texture_coord1.tv = 1;

   tex2_sprites[0].vertexes[3].position.x = 90;
   tex2_sprites[0].vertexes[3].position.y = 110;
   tex2_sprites[0].vertexes[3].position.z = 0.5f;
   tex2_sprites[0].vertexes[3].color.a = 0xff;
   tex2_sprites[0].vertexes[3].color.r = 0xff;
   tex2_sprites[0].vertexes[3].color.g = 0x00;
   tex2_sprites[0].vertexes[3].color.b = 0x00;
   tex2_sprites[0].vertexes[3].texture_coord0.tu = 1;
   tex2_sprites[0].vertexes[3].texture_coord0.tv = 0;
   tex2_sprites[0].vertexes[3].texture_coord1.tu = 1;
   tex2_sprites[0].vertexes[3].texture_coord1.tv = 0;

   tex2_sprites[0].blending0 = blending_mode_add;
   tex2_sprites[0].texture0.file_name = "banana.bmp";
   tex2_sprites[0].blending1 = blending_mode_modulate;
   tex2_sprites[0].texture1.file_name = "stain.bmp";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool a_was_pressed = false;
bool left_mb_was_pressed = false;
void handle_some_input(Input_handler& ih)
{
   bool a_is_pressed = ih.is_key_pressed(0x41);
   bool left_mb_is_pressed = ih.is_left_mouse_button_pressed();

   if (a_is_pressed && !a_was_pressed)
   {
      LOG_MAIN(Logging::minor) << "Key \'a\' pressed";
   }
   if (!a_is_pressed && a_was_pressed)
   {
      LOG_MAIN(Logging::minor) << "Key \'a\' released";
   }

   if (left_mb_is_pressed && !left_mb_was_pressed)
   {
      LOG_MAIN(Logging::minor) << "Left mouse button pressed";
   }
   if (!left_mb_is_pressed && left_mb_was_pressed)
   {
      LOG_MAIN(Logging::minor) << "Left mouse button released";
   }

   a_was_pressed = a_is_pressed;
   left_mb_was_pressed = left_mb_is_pressed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
