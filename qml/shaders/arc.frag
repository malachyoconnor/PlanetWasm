void MAIN()
{
    float arcProgress = UV0.x;
    float pulseCenter = fract(time);

    float dist = abs(arcProgress - pulseCenter);
    dist = min(dist, 1.0 - dist);

    float pulseWidth = 0.08;
    float pulse = exp(-0.5 * (dist * dist) / (pulseWidth * pulseWidth));

    float brightness = 0.35 + 0.65 * pulse;
    BASE_COLOR = vec4(baseColor.rgb * brightness, baseColor.a);
}
