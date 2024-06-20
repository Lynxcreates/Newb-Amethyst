# Pack config

# Materials to compile by default
DEFAULT_MATERIALS="RenderChunk Clouds Sky EndSky LegacyCubemap Actor SunMoon"

# Subpacks:
#  OPTIONS   = Subpack options
#  NAMES     = Names/descriptions for options
#  MATERIALS = Materials to compile for options
SUBPACK_OPTIONS=(
  FULL
  R
  DEFAULT
)
SUBPACK_NAMES=(
 "Full"
  "R"
  "Default"
)
SUBPACK_MATERIALS=(
  "Clouds"
  "RenderChunk ; Sky"
  ""
)

