<shader name="mandelbrot">
    <shaderprogram src="Data/ShaderPrograms/mandelbrot.hlsl">
        <pipelinestages>
            <vertex entrypoint="VertexFunction" />
<!--
            <hull />
            <domain />
            <geometry />
-->
            <pixel entrypoint="PixelFunction" />
            <compute entrypoint="ComputeFunction" />
        </pipelinestages>
    </shaderprogram>
    <raster>
        <fill>solid</fill>
        <cull>none</cull>
        <antialiasing>true</antialiasing>
    </raster>
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
    <depth enable="false" writable="false" />
    <stencil enable="false" readable="false" writable="false" />
</shader>