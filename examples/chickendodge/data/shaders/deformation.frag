/* Rendu du jeu */
uniform sampler2D uSampler;

/* Texture de déformation en rouge et vert */
uniform sampler2D uDeformation;

/* Texture pour contrôler l'intensité de la déformation */
uniform sampler2D uIntensity;

/* Interval de temps multiplié par la vitesse depuis l'activation du composant */
uniform float uTime;

/* Échelle de la déformation */
uniform float uScale;

/* Coordonnées UV du fragment */
varying vec2 vTextureCoord;

void main(void) {
    /*Calculer l’intensité de la déformation à appliquer selon le temps, 
    par la recherche d’une valeur dans la texture `uIntensity`, aux coordonnées `(uTime, 0.5)`. 
    Mettre cette intensité à l’échelle `uScale`.
    */
    //intensity is a bit map with greyscale values, sampling result should be [black->white->black]
    float deform_intensity = float(texture2D(uIntensity,vec2(uTime,0.5))) * uScale;
    
    
    /*
    Chercher un vecteur de déformation dans la texture `uDeformation`, 
    aux coordonnées `vTextureCoord` décalé d’une valeur tirée de `uTime` 
    (par exemple, le sinus de `uTime`). Moduler ce vecteur de déformation par l’intensité précédente.
    */
    vec4 deform_vec = texture2D(uDeformation,vTextureCoord+sin(vec2(uTime,uTime))) * deform_intensity;

    /*
    Chercher la couleur finale dans `uSampler` aux coordonnées `vTextureCoord`, décalées du vecteur de déformation.
    On soustrait par deform_vec.w / 2 qui correspond à la translation recentrée
    */
    gl_FragColor = texture2D(uSampler, vTextureCoord+deform_vec.xy - deform_vec.w / 2);
}
