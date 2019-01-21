<shader name="Test">
    <shaderprogram src="__unlit" />
    <raster src="__default" />
    <sampler src="__point" />
    <blends>
        <blend enable="true">
            <color src="src_alpha" dest="inv_src_alpha" op="add" />
        </blend>
    </blends>
</shader>