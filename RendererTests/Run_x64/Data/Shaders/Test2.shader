<shader name="Test2">
    <shaderprogram src="Data/ShaderPrograms/scanline.hlsl">
        <pipelinestages>
            <vertex entrypoint="VertexFunction" />
            <pixel entrypoint="PixelFunction" />
        </pipelinestages>
    </shaderprogram>
    <raster src="__solid" />
    <sampler src="__default" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>