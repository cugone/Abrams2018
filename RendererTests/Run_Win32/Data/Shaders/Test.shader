<shader name="Test">
    <shaderprogram src="Data/ShaderPrograms/Test.hlsl">
        <pipelinestages>
            <vertex entrypoint="VertexFunction" />
            <hull />
            <domain />
            <geometry />
            <pixel entrypoint="PixelFunction"/>
            <compute />
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