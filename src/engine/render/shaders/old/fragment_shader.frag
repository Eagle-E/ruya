#version 460 core
in vec2 ourTexCoord;
uniform vec3 objColor;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

out vec4 FragColor;

void main()
{
    /* 
    //FragColor = mix(texture(ourTexture, ourTexCoord), texture(ourTexture2, ourTexCoord), 0.2);
    
    // quick maths
    vec2 d = abs(vec2(0.5f, 0.5f) - ourTexCoord);
    float fd = sqrt(d.x*d.x + d.y*d.y);
    float md = sqrt(0.5*0.5*2);
    float p = 1-fd/md;
    
    // convert linear brightness opacity change to a parabolic one
    float i = 0.9; // intensity
    p  = -1 * pow((-p*i + 1*i), 2) + 1;
    FragColor = mix(texture(ourTexture, ourTexCoord) * p, texture(ourTexture2, ourTexCoord) * p, 0.2);
    */


    FragColor = texture(ourTexture, ourTexCoord); 
} 




// btw.. nice color: 0.2f, 0.5f, 1.0f, 1.0f