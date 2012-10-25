#version 150
uniform sampler2D weight;
uniform vec4 color;
out vec4 fragColor;
in vec2 tc;

// Uniform variable to control the usage of line coloring or brain coloring.
// If greater than zero, brain coloring is used
uniform int colorFilter;

/**
 * Affine transformation
 *
 * Transforms value, which is assumed to be between lower and upper into
 the range specified by lowerOut and upperOut */
float affine(float lower, float value, float upper, float lowerOut, float upperOut)
{
   return ((value - lower) / (upper - lower)) * (upperOut - lowerOut) + lowerOut;
}

vec4 brainColor()
{
   vec4 fragColor = texture(weight, tc);
   if (fragColor.r <= -100)
   {
      fragColor = vec4(1,1,1,1);
   }
   else if (fragColor.r <= -99)
   {
      fragColor = vec4(.5,.5,.5,1);
   }
   else if (fragColor.r <= -98)
   {
      fragColor = vec4(0,0,0,1);
   }
   else if (fragColor.r <= -97)
   {
      fragColor = vec4(0,1,1,1);
   }
   else
   {
      if(fragColor.r < .5)
      {
         fragColor = fragColor.r * color;
      }
      else
      {
         float distance = affine(0.5, fragColor.r, 1.0, 0, 1);
         vec3 color = mix(color.rgb, vec3(1,1,0), distance);
         fragColor = vec4(color, 1);
      }
   }
   return fragColor;
}

vec4 lineColor()
{
   // This is where Devon gets to be creative and make up some
   // fancy line coloring scheme
   return vec4(0,0,0,1);
}

void main(void)
{
   if(colorFilter > 0)
   {
      fragColor = brainColor();
   }
   else
   {
      fragColor = lineColor();
   }
}
