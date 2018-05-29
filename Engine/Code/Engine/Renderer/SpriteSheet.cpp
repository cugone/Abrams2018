#include "Engine/Renderer/SpriteSheet.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"

SpriteSheet::SpriteSheet(Renderer& renderer, const XMLElement& elem)
{
    LoadFromXml(renderer, elem);
}

SpriteSheet::SpriteSheet(const Texture* texture, int tilesWide, int tilesHigh)
    : _spriteSheetTexture(texture)
    , _spriteLayout(tilesWide, tilesHigh)
{
    /* DO NOTHING */
}

SpriteSheet::SpriteSheet(Renderer& renderer, const std::string& texturePath, int tilesWide, int tilesHigh)
    : _spriteSheetTexture(renderer.CreateOrGetTexture(texturePath, IntVector3::XY_AXIS))
    , _spriteLayout(tilesWide, tilesHigh)
{

}

SpriteSheet::~SpriteSheet() {
    _spriteSheetTexture = nullptr;
}

AABB2 SpriteSheet::GetTexCoordsFromSpriteCoords(int spriteX, int spriteY) const {
    Vector2 texCoords(1.0f / _spriteLayout.x, 1.0f / _spriteLayout.y);
    return AABB2(Vector2(texCoords.x * spriteX, texCoords.y * spriteY), Vector2(texCoords.x * (spriteX + 1), texCoords.y * (spriteY + 1)));
}

AABB2 SpriteSheet::GetTexCoordsFromSpriteCoords(const IntVector2& spriteCoords) const {
    return GetTexCoordsFromSpriteCoords(spriteCoords.x, spriteCoords.y);
}

AABB2 SpriteSheet::GetTexCoordsFromSpriteIndex(int spriteIndex) const {
    int x = spriteIndex % _spriteLayout.x;
    int y = spriteIndex / _spriteLayout.y;
    return GetTexCoordsFromSpriteCoords(x, y);
}

int SpriteSheet::GetNumSprites() const {
    return ((*_spriteSheetTexture).GetDimensions().x / _spriteLayout.x) * _spriteLayout.y;
}

const Texture& SpriteSheet::GetTexture() const {
    return *_spriteSheetTexture;
}

void SpriteSheet::LoadFromXml(Renderer& renderer, const XMLElement& elem) {
    DataUtils::ValidateXmlElement(elem, "spritesheet", "", "src,dimensions");
    _spriteLayout = DataUtils::ParseXmlAttribute(elem, "dimensions", _spriteLayout);
    std::string texturePath{};
    texturePath = DataUtils::ParseXmlAttribute(elem, "src", texturePath);
    _spriteSheetTexture = renderer.CreateOrGetTexture(texturePath, IntVector3::XY_AXIS);
}
