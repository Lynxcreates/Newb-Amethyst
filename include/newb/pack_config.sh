# Pack config

# Materials to compile by default
DEFAULT_MATERIALS="RenderChunk Clouds Sky EndSky LegacyCubemap Actor SunMoon"

# Subpacks:
#  OPTIONS   = Subpack options
#  NAMES     = Names/descriptions for options
#  MATERIALS = Materials to compile for options
SUBPACK_OPTIONS=(
  FANTASY
  HORROR
  FULL
  R
  DEFAULT
)
SUBPACK_NAMES=(
  "Fantasy"
  "Horror"
 "Full"
  "R"
  "Default"
)
SUBPACK_MATERIALS=(
  "RenderChunk ; Clouds ;  Sky ; EndSky ; LegacyCubemap"
  "RenderChunk ; Clouds ;  Sky ; EndSky ; LegacyCubemap"
  "Clouds"
  "RenderChunk ; Sky"
  ""
)

