#define PI 3.1415926538

in vec3 texCoord;


// textures
uniform sampler3D scalar_field_3D;
uniform sampler3D vector_field_3D;
uniform sampler2D noise_field;

// TODO: =======================================================================
//
// Add custom uniforms and constants here
//
// =============================================================================

void main()
{

    // TODO: ===================================================================
    //
    // Implement LIC and colorcoded scalar field overlay
    //
    // =========================================================================
    // remove this line, and replace it with your solution
    gl_FragColor = texture3D(scalar_field_3D, texCoord.xyz);
}