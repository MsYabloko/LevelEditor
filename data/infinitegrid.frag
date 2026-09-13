#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform vec3 cameraWorldPos;

uniform float gGridCellSize = 1;

const float gGridMinPixelsBetweenCells = 2.0;
const vec4 gGridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 gGridColorThick = vec4(1.0, 1.0, 1.0, 1.0);

vec2 satv(vec2 x)
{
   vec2 v = clamp(x, vec2(0.0), vec2(1.0));
   return v;
}
float satf(float x)
{
   float f = clamp(x, 0.0, 1.0);
   return f;
}
float log10(float x)
{
   float f = log(x) / log(10.0);
   return f;
}

void main()
{
   vec2 dvx = vec2(dFdx(WorldPos.x), dFdy(WorldPos.x));
   vec2 dvy = vec2(dFdx(WorldPos.z), dFdy(WorldPos.z));
   float lx = length(dvx);
   float ly = length(dvy);
   vec2 dudv = vec2(lx, ly);

   float l = length(dudv);
   float LOD = max(0.0, log10(l * gGridMinPixelsBetweenCells / gGridCellSize) + 1.0);
   float GridCellSizeLod0 = gGridCellSize * pow(10.0, floor(LOD));
   float GridCellSizeLod1 = GridCellSizeLod0 * 10;
   float GridCellSizeLod2 = GridCellSizeLod1 * 10;

   dudv *= 4.0;

   vec2 mod_div_dudv = mod(WorldPos.xz, GridCellSizeLod0) / dudv;
   mod_div_dudv = vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0));
   float Lod0a = max(mod_div_dudv.x, mod_div_dudv.y);
   float onlyX0 = mod_div_dudv.x;
   float onlyY0 = mod_div_dudv.y;

   mod_div_dudv = mod(WorldPos.xz, GridCellSizeLod1) / dudv;
   mod_div_dudv = vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0));
   float Lod1a = max(mod_div_dudv.x, mod_div_dudv.y);
   float onlyX1 = mod_div_dudv.x;
   float onlyY1 = mod_div_dudv.y;

   mod_div_dudv = mod(WorldPos.xz, GridCellSizeLod2) / dudv;
   mod_div_dudv = vec2(1.0) - abs(satv(mod_div_dudv) * 2.0 - vec2(1.0));
   float Lod2a = max(mod_div_dudv.x, mod_div_dudv.y);
   float onlyX2 = mod_div_dudv.x;
   float onlyY2 = mod_div_dudv.y;

   float LOD_fade = fract(LOD);

   vec4 Color;

   float onlyX;
   float onlyY;
   float finalCellSize = GridCellSizeLod0;
   if(abs(cameraWorldPos.y) < 1 && (distance(WorldPos, cameraWorldPos) > 20 || abs(cameraWorldPos.y) < 0.25f)) discard;
   if(Lod2a > 0.0)
   {
      Color = gGridColorThick;
      Color.a *= Lod2a;
      onlyX = onlyX2;
      onlyY = onlyY2;
      finalCellSize = GridCellSizeLod2;
   }
   else
   {
      if(Lod1a > 0.0)
      {
         Color = mix(gGridColorThick, gGridColorThin, LOD_fade);
         Color.a *= Lod1a;
         onlyX = onlyX1;
         onlyY = onlyY1;
         finalCellSize = GridCellSizeLod1;
      }
      else
      {
         Color = gGridColorThin;
         Color.a *= (Lod0a * (1.0 - LOD_fade));
         onlyX = onlyX0;
         onlyY = onlyY0;
      }
   }
   if(Color.a < 0.1) discard;

   if(abs(WorldPos.x) < finalCellSize / 2 && onlyX >= onlyY)
   {
      Color = vec4(0, 0, 1, Color.a);
   }
   if(abs(WorldPos.z) < finalCellSize / 2 && onlyY >= onlyX)
   {
      Color = vec4(1, 0, 0, Color.a);
   }
   float OpacityFallof = (1.0 - satf(length(WorldPos.xz - cameraWorldPos.xz) / 95));
   Color.a *= OpacityFallof;

   FragColor = Color;
}