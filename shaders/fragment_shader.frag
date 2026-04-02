#version 430 core
    in vec3 WorldPos;
    in vec3 InstancePos;
    out vec4 FragColor;
    uniform float uTime;
    uniform int uGridSize; // <-- EN INT POUR MATCHER LE VERTEX SHADER
    void main()
    {
       float gridSizeF = float(uGridSize); // Conversion pour les calculs ici !
       
       vec3 dFdxPos = dFdx(WorldPos);
       vec3 dFdyPos = dFdy(WorldPos);
       vec3 normal = normalize(cross(dFdxPos, dFdyPos));
       
       float yNorm = (InstancePos.y / gridSizeF) * 0.5 + 0.5;
       float xNorm = (InstancePos.x / gridSizeF) * 0.5 + 0.5;
       float zNorm = (InstancePos.z / gridSizeF) * 0.5 + 0.5;
       
       vec3 colorCyan = vec3(0.0, 0.8, 1.0);
       vec3 colorMagenta = vec3(1.0, 0.2, 0.7);
       vec3 colorGold = vec3(1.0, 0.8, 0.1);
       
       vec3 baseColor = (yNorm < 0.5) ? mix(colorCyan, colorMagenta, yNorm * 2.0) : mix(colorMagenta, colorGold, (yNorm - 0.5) * 2.0);
       baseColor = mix(baseColor, vec3(1.0, 0.1, 0.3), abs(InstancePos.x) / gridSizeF * 0.1);
       
       vec3 lightDir = normalize(vec3(1.0, 1.5, 0.5));
       float diffuse = max(dot(normal, lightDir), 0.0);
       
       vec3 pointLightPos = vec3(0.0, -10.0, 0.0);
       float distToPoint = length(InstancePos - pointLightPos);
       float pointLightIntensity = pow(1.0 / (1.0 + 0.01 * distToPoint * distToPoint), 2.0);
       vec3 pointColor = vec3(1.0, 0.2, 0.7);
       vec3 emission = baseColor * 0.4;
       
       float lightAmb = 0.8;
       vec3 litColor = baseColor * (lightAmb + diffuse * 0.7) + (pointColor * pointLightIntensity * 15.0) + emission;
       
       float dist = length(InstancePos);
       float fogFactor = exp(-dist / (gridSizeF * 1.8));
       vec3 cosmicBackground = vec3(0.005, 0.005, 0.01);
       
       vec3 finalColor = mix(cosmicBackground, litColor, clamp(fogFactor, 0.0, 1.0));
       FragColor = vec4(finalColor, 1.0f);
    }