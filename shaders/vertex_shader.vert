#version 430 core
    layout (location = 0) in vec3 vPos;
    layout(std430, binding = 0) buffer InBuffer { int gridIn[]; };
    out vec3 WorldPos;
    out vec3 InstancePos;
    uniform mat4 VP;
    uniform int uGridSize;
    uniform float uSpacing;
    
    // Fonction magique pour générer de l'aléatoire sur le GPU (entre 0.0 et 1.0)
    float random(vec3 co) {
        return fract(sin(dot(co, vec3(12.9898, 78.233, 54.53))) * 43758.5453);
    }
    
    void main()
    {
       int id = gl_InstanceID;
       int x = id % uGridSize;
       int y = (id / uGridSize) % uGridSize;
       int z = id / (uGridSize * uGridSize);
       
       int alive = gridIn[id];
       float scale = (alive == 1) ? 1.0 : 0.0;
       
       vec3 logicalPos = vec3(x, y, z);
       
       // 1. On crée un décalage aléatoire (entre -0.5 et 0.5 pour chaque axe)
       vec3 jitter = vec3(
           random(logicalPos) - 0.5,
           random(logicalPos + vec3(1.0)) - 0.5,
           random(logicalPos + vec3(2.0)) - 0.5
       );
       
       // 2. On multiplie ce décalage pour qu'il soit proportionnel à l'espacement
       // (Ici 0.8 * uSpacing, modifie le 0.8 si tu veux que ce soit plus ou moins chaotique)
       jitter *= uSpacing * 0.8;
     jitter *= 0.0;
       
       // 3. On ajoute le jitter à la position finale !
       vec3 offset = (logicalPos - vec3(float(uGridSize) / 2.0)) * uSpacing + jitter;
       vec3 pos = (vPos * scale) + offset;
       
       WorldPos = pos;
       InstancePos = offset;
       gl_Position = VP * vec4(pos, 1.0);
    }