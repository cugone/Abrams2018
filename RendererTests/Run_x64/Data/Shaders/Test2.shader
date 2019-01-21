<shader name="Test2">
    <!--<shaderprogram src="__unlit">-->
    <shaderprogram src="Data/ShaderPrograms/mandelbrot.hlsl">
        <pipelinestages>
            <vertex entrypoint="VertexFunction" />
            <pixel entrypoint="PixelFunction" />
            <compute entrypoint="ComputeFunction" />
        </pipelinestages>
    </shaderprogram>
    <raster src="__solid" />
    <sampler src="__point" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>