// CS 247 - Scientific Visualization, KAUST
//
// Programming Assignment #6
#include <cstring>
#include "CS247_prog.h"

// cycle clear colors
static void nextClearColor()
{
    clearColor = (++clearColor) % 3;

    switch(clearColor)
    {
        case 0:
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case 1:
            glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
            break;
        default:
            glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
            break;
    }
}


// callbacks
// framebuffer to fix viewport
void frameBufferCallback(GLFWwindow* window, int width, int height)
{
    view_width = width;
    view_height = height;
    glViewport(0, 0, width, height);
}

// key callback to take user inputs for both windows
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_RELEASE) {
        char* status[ 2 ];
        status[ 0 ] = "disabled";
        status[ 1 ] = "enabled";

        switch(key) {
            case GLFW_KEY_1:
                toggle_xy = 0;
                LoadData( filenames[ 0 ] );
                loaded_file = 0;
                fprintf( stderr, "Loading " );
                fprintf( stderr, filenames[ 0 ] );
                fprintf( stderr, " dataset.\n");
                break;
            case GLFW_KEY_2:
                toggle_xy = 0;
                LoadData(filenames[ 1 ] );
                loaded_file = 1;
                fprintf( stderr, "Loading " );
                fprintf( stderr, filenames[ 1 ] );
                fprintf( stderr, " dataset.\n");
                break;
            case GLFW_KEY_3:
                toggle_xy = 1;
                LoadData( filenames[ 2 ] );
                loaded_file = 2;
                fprintf( stderr, "Loading " );
                fprintf( stderr, filenames[ 2 ] );
                fprintf( stderr, " dataset.\n");
                break;
            case GLFW_KEY_0:
                if( num_timesteps > 1 ){
                    NextTimestep();
                    fprintf( stderr, "Timestep %d.\n", current_timestep );
                }
                break;
            case GLFW_KEY_R:
                scalar_overlay = 1 - scalar_overlay;
                fprintf( stderr, "Scalar field in background %s.\n", status[ scalar_overlay ] );
                break;
            case GLFW_KEY_I:
                enable_autocycle = 1 - enable_autocycle;
                fprintf( stderr, "Autocycling %s.\n", status[ enable_autocycle ] );
                break;
            case GLFW_KEY_N:
                scalar_field_id = ( scalar_field_id + 1 ) % num_scalar_fields;
                DownloadScalarFieldAs3DTexture();
                fprintf( stderr, "Next scalar field.\n");
                break;
            case GLFW_KEY_V:
                FreeTextures();
                ResetRenderingProperties();
                InitTextures();
                break;
            case GLFW_KEY_B:
                nextClearColor();
                fprintf( stderr, "Next clear color.\n");
                break;
            case GLFW_KEY_Y:
                blend_factor = std::min(1., blend_factor+0.05);
                fprintf( stderr, "Blend factor increased to %.2f\n", blend_factor);
                break;
            case GLFW_KEY_H:
                blend_factor = std::max(0., blend_factor-0.05);
                fprintf( stderr, "Blend factor decreased to %.2f\n", blend_factor);
                break;
            case GLFW_KEY_T:
                dt = std::min(1., dt+0.001);
                fprintf( stderr, "dt increased to %.5f\n", dt);
                break;
            case GLFW_KEY_G:
                dt = std::max(.0001, dt-0.001);
                fprintf( stderr, "dt decreased to %.5f\n", dt);
                break;
            case GLFW_KEY_EQUAL:
                kernel_size = kernel_size + 2;
                fprintf( stderr, "kernel size increased to %d\n", kernel_size);
                break;
            case GLFW_KEY_MINUS:
                kernel_size = (int)std::max(3., kernel_size-2.);
                fprintf( stderr, "kernel size decreased to %d\n", kernel_size);
                break;
            case GLFW_KEY_C:
                colormap = 1 - colormap;
                fprintf( stderr, "colormap %d.\n", colormap );
                break;
            case GLFW_KEY_Q:
            case 27:
                exit( 0 );
                break;
            default:
                fprintf( stderr, "\nKeyboard commands:\n\n"
                                 "1, load %s dataset\n"
                                 "2, load %s dataset\n"
                                 "3, load %s dataset\n"
                                 "0, cycle through timesteps\n"
                                 "i, toggle autocycling between timesteps\n"
                                 "r, enable scalar field visualization\n"
                                 "n, next scalar field\n"
                                 "b, switch background color\n"
                                 "q, <esc> - Quit\n",
                         filenames[0], filenames[1], filenames[2] );
                break;
        }
    }
}


// glfw error callback
static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


// data

void NextTimestep( void )
{
    if( num_timesteps == 0 ) return;

    current_timestep = ( current_timestep + 1 ) % num_timesteps;
    glDeleteTextures( 1, &scalar_field_texture_3D );
    glDeleteTextures( 1, &vector_field_texture_3D );

    DownloadScalarFieldAs3DTexture();
    DownloadVectorFieldAs3DTexture();
}

/*
 * load .gri dataset
 */
void LoadData( char* base_filename )
{
    //reset
    ResetRenderingProperties();

    // free textures
    FreeTextures();

    //reset data
    if(!scalar_fields){

        delete[] scalar_fields;
        scalar_fields = NULL;

        delete[] scalar_bounds;
        scalar_bounds = NULL;
    }

    if(!vector_fields){

        delete[] vector_fields;
        vector_fields = NULL;
        delete[] vector_fields_tex;
        vector_fields_tex = NULL;
    }

    char filename[ 80 ];
    strcpy( filename, base_filename );
    strcat( filename, ".gri");

    fprintf( stderr, "loading grid file %s\n", filename );

    // open grid file, read only, binary mode
    FILE* fp = fopen( filename, "rb" );
    if ( fp == NULL ) {
        fprintf( stderr, "Cannot open file %s for reading.\n", filename );
        return;
    }

    // read header
    char header[ 40 ];
    fread( header, sizeof( char ), 40, fp );
    sscanf( header, "SN4DB %d %d %d %d %d %f",
            &vol_dim[ 0 ], &vol_dim[ 1 ], &vol_dim[ 2 ],
            &num_scalar_fields, &num_timesteps ,&timestep );

    fprintf( stderr, "dimensions: x: %d, y: %d, z: %d.\n", vol_dim[ 0 ], vol_dim[ 1 ], vol_dim[ 2 ] );
    fprintf( stderr, "additional info: # scalar fields: %d, # timesteps: %d, timestep: %f.\n", num_scalar_fields, num_timesteps, timestep );

    scalar_fields = new float[  vol_dim[ 0 ] * vol_dim[ 1 ] * num_timesteps * num_scalar_fields ];
    scalar_bounds = new float[  2 * num_timesteps * num_scalar_fields ];
    vector_fields = new vec3[ vol_dim[ 0 ] * vol_dim[ 1 ] * num_timesteps ];
    vector_fields_tex = new vec3[ vol_dim[ 0 ] * vol_dim[ 1 ] * num_timesteps ];

    if( num_timesteps <= 1 ){

        current_timestep = 0;

        char dat_filename[ 80 ];
        strcpy( dat_filename, base_filename );
        strcat( dat_filename, ".dat");

        LoadVectorData( dat_filename );

    } else {

        for( int i = 0; i < num_timesteps; i++ ){

            current_timestep = i;

            char dat_filename[ 80 ];
            sprintf( dat_filename, "%s.%.5d.dat", base_filename, current_timestep );

            LoadVectorData( dat_filename );
        }

        current_timestep = 0;
    }

    CreateNoiseField();

    InitTextures();

    glfwSetWindowSize(window, vol_dim[ 0 ], vol_dim[ 1 ] );

    grid_data_loaded = true;
}

/*
 * load .dat dataset
 */
void LoadVectorData( const char* filename )
{
    fprintf( stderr, "loading scalar file %s\n", filename );

    // open data file, read only, binary mode
    FILE* fp = fopen( filename, "rb" );
    if ( fp == NULL ) {
        fprintf( stderr, "Cannot open file %s for reading.\n", filename );
        return;
    }

    int data_size = vol_dim[ 0 ] * vol_dim[ 1 ];

    int num_total_fields = num_scalar_fields + 3;

    // read scalar data
    float *tmp = new float[ data_size * num_total_fields ];
    fread( tmp, sizeof( float ), ( data_size * ( num_total_fields ) ), fp );

    // close file
    fclose( fp );

    // vec min max;
    float min_val = 99999.9f;
    float max_val = 0.0f;

    int t_step_off = current_timestep * data_size;

    // copy vector data
    for( int i = 0; i < data_size; i++ ){

        vec3 val;
        if( toggle_xy ) {
            val.x = tmp[ i * num_total_fields + 1 ];  //toggle x and y components in vector field
            val.y = tmp[ i * num_total_fields + 0 ];
            val.z = tmp[ i * num_total_fields + 2 ];

        } else {
            val.x = tmp[ i * num_total_fields + 0 ];
            val.y = tmp[ i * num_total_fields + 1 ];
            val.z = tmp[ i * num_total_fields + 2 ];
        }

        vector_fields[ t_step_off + i ] = val;

        min_val = std::min( std::min( std::min( val.x, val.y ), val.z ), min_val );
        max_val = std::max( std::max( std::max( val.x, val.y ), val.z ), max_val );
    }

    // scale vectors
    // scale between [0..1] where 1 is original zero
    // the boundary of the bigger abs border will be used to scale
    // meaning one boundary will likely not be hit i.e real scale
    // for -50..100 will be [0.25..1.0]
    if( min_val < 0.0 && max_val > 0.0 ){

        float scale = 0.5f / std::max( -min_val, max_val );

        for( int j = t_step_off; j < t_step_off + data_size; j++ ){

            vector_fields_tex[ j ].x = 0.5f + vector_fields[ j ].x * scale;
            vector_fields_tex[ j ].y = 0.5f + vector_fields[ j ].y * scale;
            vector_fields_tex[ j ].z = 0.5f + vector_fields[ j ].z * scale;
        }
        // scale between [0..1]
    } else {

        float sign = max_val <= 0.0 ? -1.0f : 1.0f;
        float scale = 1.0f / ( max_val - min_val ) * sign;

        for( int j = t_step_off; j < t_step_off + data_size; j++ ){

            vector_fields_tex[ j ].x = ( vector_fields[ j ].x - min_val ) * scale;
            vector_fields_tex[ j ].y = ( vector_fields[ j ].y - min_val ) * scale;
            vector_fields_tex[ j ].z = ( vector_fields[ j ].z - min_val ) * scale;
        }
    }


    // copy scalar data
    int sc_field_size = data_size * num_timesteps;
    for( int  i = 0; i < num_scalar_fields; i++ ){

        int off = i * sc_field_size + t_step_off;

        min_val = 99999.9f;
        max_val = 0.0f;

        for( int j = 0; j < data_size; j++ ){

            float val = tmp[ j * num_total_fields + 3 + i ];

            scalar_fields[ off + j ] = val;

            min_val = std::min( val, min_val );
            max_val = std::max( val, max_val );
        }

        scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 0 ] = min_val;
        scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 1 ] = max_val;
    }


    // normalize scalars
    for( int  i = 0; i < num_scalar_fields; i++ ){

        int off = i * sc_field_size + t_step_off;

        float lower_bound = scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 0 ];
        float upper_bound = scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 1 ];

        // scale between [0..1] where 1 is original zero
        // the boundary of the bigger abs border will be used to scale
        // meaning one boundary will likely not be hit i.e real scale
        // for -50..100 will be [0.25..1.0]
        if( lower_bound < 0.0 && upper_bound > 0.0 ){

            float scale = 0.5f / std::max( -lower_bound, upper_bound );

            for( int j = 0; j < data_size; j++ ){

                scalar_fields[ off + j ] = 0.5f + scalar_fields[ off + j ] * scale;
            }
            scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 0 ] = 0.5f + scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 0 ] * scale;
            scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 1 ] = 0.5f + scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 1 ] * scale;


            // scale between [0..1]
        } else {

            float sign = upper_bound <= 0.0 ? -1.0f : 1.0f;

            float scale = 1.0f / ( upper_bound - lower_bound ) * sign;

            for( int j = 0; j < data_size; j++ ){

                scalar_fields[ off + j ] = ( scalar_fields[ off + j ] - lower_bound ) * scale;
            }
            scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 0 ] = ( scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 0 ] + lower_bound ) * scale; //should be 0.0
            scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 1 ] = ( scalar_bounds[ i * 2 * num_timesteps + 2 * current_timestep + 1 ] + lower_bound ) * scale; //should be 1.0
        }
    }

    delete[] tmp;

    scalar_data_loaded = true;
}

void CreateNoiseField( void )
{
    if(!noise_field){

        delete[] noise_field;
        noise_field = NULL;
    }
    int img_size = vol_dim[ 0 ] * vol_dim[ 1 ];
    noise_field = new float[ img_size ];

    // TODO: =======================================================================
    //
    // Initialize white noise field
    //
    // =============================================================================

}

void InitTextures( void ){

    DownloadVectorFieldAs3DTexture();
    DownloadScalarFieldAs3DTexture();
    DownloadNoiseFieldAs2DTexture();
}

void FreeTextures( void ){

    glDeleteTextures( 1, &scalar_field_texture_3D );
    glDeleteTextures( 1, &vector_field_texture_3D );
    glDeleteTextures( 1, &noise_field_texture );
}

void DownloadScalarFieldAs3DTexture( void )
{
    fprintf( stderr, "downloading scalar field to 3D texture\n" );

    // generate and bind 2D texture
    glGenTextures( 1, &scalar_field_texture_3D );
    glBindTexture( GL_TEXTURE_3D, scalar_field_texture_3D );

    // set necessary texture parameters
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    int scalar_field_off = scalar_field_id * vol_dim[ 0 ] * vol_dim[ 1 ] * num_timesteps + current_timestep * vol_dim[ 0 ] * vol_dim[ 1 ];
    //download texture in correct format
    glTexImage3D( GL_TEXTURE_3D, 0,  GL_INTENSITY16, vol_dim[ 0 ], vol_dim[ 1 ], 1, 0, GL_LUMINANCE, GL_FLOAT, &(scalar_fields[ scalar_field_off ]) );

}

void DownloadNoiseFieldAs2DTexture( void )
{
    fprintf( stderr, "downloading noise field to 2D texture\n" );

    // generate and bind 2D texture
    glGenTextures( 1, &noise_field_texture );
    glBindTexture( GL_TEXTURE_2D, noise_field_texture );

    // set necessary texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    //download texture in correct format
    glTexImage2D( GL_TEXTURE_2D, 0,  GL_LUMINANCE16, vol_dim[ 0 ], vol_dim[ 1 ], 0, GL_LUMINANCE, GL_FLOAT, noise_field );
}

void DownloadVectorFieldAs3DTexture( void )
{
    fprintf( stderr, "downloading vector field to 3D texture\n" );

    // generate and bind 2D texture
    glGenTextures( 1, &vector_field_texture_3D );
    glBindTexture( GL_TEXTURE_3D, vector_field_texture_3D );

    // set necessary texture parameters
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );

    //download texture in correct format
    glTexImage3D( GL_TEXTURE_3D, 0,  GL_RGB16, vol_dim[ 0 ], vol_dim[ 1 ], 1, 0, GL_RGB, GL_FLOAT, &vector_fields_tex[vol_dim[ 0 ] * vol_dim[ 1 ] * current_timestep] );
}

char *textFileRead(char *fn) {

    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fn != NULL) {
        fp = fopen(fn, "rt");
        if (fp != NULL) {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

// initializations
// init application
bool initApplication(int argc, char **argv)
{

    std::string version((const char *)glGetString(GL_VERSION));
    std::stringstream stream(version);
    unsigned major, minor;
    char dot;

    stream >> major >> dot >> minor;

    assert(dot == '.');
    if (major > 3 || (major == 2 && minor >= 0)) {
        std::cout << "OpenGL Version " << major << "." << minor << std::endl;
    } else {
        std::cout << "The minimum required OpenGL version is not supported on this machine. Supported is only " << major << "." << minor << std::endl;
        return false;
    }

    return true;
}

void ResetRenderingProperties( void )
{
    num_scalar_fields = 0;
    scalar_field_id = 0;

    scalar_overlay = 0;

    enable_autocycle = 0;
}


// set up the scene
void setup() {
    LoadData( filenames[ 0 ] );
    loaded_file = 0;


    // compile & link shader
    LICProgram.compileShader("../shaders/vertex.vs");
    LICProgram.compileShader("../shaders/lic.fs");
    LICProgram.link();

    // make quad to render texture
    // see: vboquad.h and vboquad.cpp
    quad.init();
}

// rendering
void render() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    LICProgram.use();
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_3D, scalar_field_texture_3D );
    LICProgram.setUniform("scalar_field_3D", 1);

    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, noise_field_texture );
    LICProgram.setUniform("noise_field", 2);

    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_3D, vector_field_texture_3D );
    LICProgram.setUniform("vector_field_3D", 3);

    glActiveTexture( GL_TEXTURE0 );

    // TODO: =======================================================================
    //
    // Add needed uniforms here (you probably need at least the size of a texel,
    // a flag whether to overlay a scalar field or not, the LIC kernel size ...
    //
    // =============================================================================

    quad.render();
}

// entry point
int main(int argc, char** argv)
{
    // init variables
    view_width = 0;
    view_height = 0;

    toggle_xy = 0;

    kernel_size = 59;
    dt = 0.002;
    blend_factor = 0.5;
    colormap = 0;

    ResetRenderingProperties();

    vector_fields = NULL;
    vector_fields_tex = NULL;
    scalar_fields = NULL;
    scalar_bounds = NULL;
    grid_data_loaded = false;
    scalar_data_loaded = false;

    noise_field = NULL;


    filenames[ 0 ] = "../data/block/c_block";
    filenames[ 1 ] = "../data/tube/tube";
    filenames[ 2 ] = "../data/hurricane/hurricane_p_tc";



    // set glfw error callback
    glfwSetErrorCallback(errorCallback);

    // init glfw
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // init glfw window
    window = glfwCreateWindow(gWindowWidth, gWindowHeight, "CS247 Scientific Visualization", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // set GLFW callback functions
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, frameBufferCallback);

    // make context current (once is sufficient)
    glfwMakeContextCurrent(window);

    // get the frame buffer size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // init the OpenGL API (we need to do this once before any calls to the OpenGL API)
    gladLoadGL();

    // init our application
    if (!initApplication(argc, argv)) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


    // set up the scene
    setup();

    // print menu
    keyCallback(window, GLFW_KEY_BACKSLASH, 0, GLFW_PRESS, 0);

    // start traversing the main loop
    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // clear frame buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render one frame
        render();

        // swap front and back buffers
        glfwSwapBuffers(window);

        // poll and process input events (keyboard, mouse, window, ...)
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}