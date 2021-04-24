#include "font_engine_factory.h"
#include "font_engine.h"
#include "font_engine_default.h"
#include "font_engine_freetype.h"
#include "reverse_lock.h"

#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>

namespace opengl {
namespace font_engine_factory {

std::unordered_map<
   std::string,
   std::weak_ptr< FontEngine > >
   font_engines_;

std::mutex
   font_engines_mutex_;

std::shared_ptr< FontEngine >
CreateFontEngine(
   std::unique_lock< std::mutex > & lock,
   const std::string & font_filename,
   const uint32_t size,
   const FontEngineType type ) noexcept
{
   reverse_lock rlock {
      *lock.mutex() };

   std::shared_ptr< FontEngine > font_engine {
      nullptr
   };

   switch (type)
   {
   case FontEngineType::DEFAULT:
      font_engine =
         std::make_shared< FontEngineDefault >();

      break;
   
   case FontEngineType::FREETYPE:
      font_engine =
         std::make_shared< FontEngineFreeType >();
      
      break;
   }

   if (font_engine)
   {
      const bool initialized =
         font_engine->Initialize();

      if (!initialized)
      {
         font_engine.reset();
      }
      else
      {
         const bool set =
            font_engine->SetFont(
               font_filename,
               size);

         if (!set &&
             type != FontEngineType::DEFAULT)
         {
            font_engine.reset();
         }
      }
   }

   return
      font_engine;
}

std::shared_ptr< FontEngine >
ConstructFontEngine(
   const std::string & font_filename,
   const uint32_t size,
   const FontEngineType type ) noexcept
{
   std::shared_ptr< FontEngine >
      font_engine;

   std::stringstream ss_index;

   ss_index
      << std::this_thread::get_id() << " "
      << font_filename << " "
      << size << " "
      << static_cast< size_t >(type);

   const std::string index =
      ss_index.str();

   std::unique_lock lock {
      font_engines_mutex_ };

   auto font_engine_it =
      font_engines_.find(index);

   if (font_engine_it == font_engines_.cend() ||
       font_engine_it->second.expired())
   {
      font_engine =
         CreateFontEngine(
            lock,
            font_filename,
            size,
            type);

      if (!font_engine &&
          type != FontEngineType::DEFAULT)
      {
         reverse_lock rlock {
            lock };

         font_engine =
            ConstructFontEngine(
               font_filename,
               size,
               FontEngineType::DEFAULT);
      }

      font_engine_it =
         font_engines_.emplace(
            index,
            std::weak_ptr< FontEngine > { }).first;

      font_engine_it->second =
         std::weak_ptr< FontEngine > {
            font_engine };
   }
   else
   {
      font_engine =
         font_engine_it->second.lock();
   }

   return
      font_engine;
}

}} // namespace opengl::font_engine_factory
