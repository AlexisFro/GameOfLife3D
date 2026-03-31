#version 430 core

in vec3 WorldPos;
in vec3 InstancePos;

out vec4 FragColor;

uniform float uTime;
uniform float uGridSize;

void main()
{
    // 1. FLAT SHADING : Calcul des normales à la volée
    // dFdx et dFdy calculent la différence de position entre les pixels voisins
    vec3 dFdxPos = dFdx(WorldPos);
    vec3 dFdyPos = dFdy(WorldPos);
    vec3 normal = normalize(cross(dFdxPos, dFdyPos));
    
    // 2. PALETTE DE COULEURS : Dégradé Spatial vibrant
    float yNorm = (InstancePos.y / uGridSize) * 0.5 + 0.5; // [-1, 1] -> [0, 1]
    
    // Couleurs clés
    vec3 colorCyan    = vec3(0.0, 0.8, 1.0);  // Bas / Froid
    vec3 colorMagenta = vec3(1.0, 0.2, 0.7);  // Milieu / Vibrant
    vec3 colorGold    = vec3(1.0, 0.8, 0.1);  // Haut / Chaud
    
    // Mixer les couleurs sur l'axe Y
    vec3 baseColor = (yNorm < 0.5) 
        ? mix(colorCyan, colorMagenta, yNorm * 2.0) 
        : mix(colorMagenta, colorGold, (yNorm - 0.5) * 2.0);
        
    // Variation subtile sur X pour plus de richesse
    baseColor = mix(baseColor, vec3(1.0, 0.1, 0.3), abs(InstancePos.x) / uGridSize * 0.1);
    
    // 3. ÉCLAIRAGE DIRECTIONNEL (Soleil)
    vec3 lightDir = normalize(vec3(1.0, 1.5, 0.5));
    float diffuse = max(dot(normal, lightDir), 0.0);
    
    // 4. ÉCLAIRAGE POINT CENTRAL (Lueur interne)
    vec3 pointLightPos = vec3(0.0, -10.0, 0.0);
    float distToPoint = length(InstancePos - pointLightPos);
    float pointLightIntensity = 1.0 / (1.0 + 0.01 * distToPoint * distToPoint);
    pointLightIntensity = pow(pointLightIntensity, 2.0); 
    vec3 pointColor = vec3(1.0, 0.2, 0.7);
    
    // 5. Matériau : Émission
    vec3 emission = baseColor * 0.4;
    
    // 6. Combinaison finale
    float lightAmb = 0.3;
    vec3 litColor = baseColor * (lightAmb + diffuse * 0.7) 
                  + (pointColor * pointLightIntensity * 15.0) 
                  + emission;
    
    // 7. BROUILLARD (Cosmic Fog)
    float dist = length(InstancePos);
    float fogFactor = exp(-dist / (uGridSize * 1.8)); 
    vec3 cosmicBackground = vec3(0.005, 0.005, 0.01); // Noir bleuté
    
    vec3 finalColor = mix(cosmicBackground, litColor, clamp(fogFactor, 0.0, 1.0));
    
    FragColor = vec4(finalColor, 1.0);
}