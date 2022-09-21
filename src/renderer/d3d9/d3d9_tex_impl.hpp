class d3d9_texture_t : public texture_t {
  public:
  LPDIRECT3DTEXTURE9 texture;

  access_info_t access(int x, int y, int width, int height) {
    RECT r = { x, y, x + width, y + height };

    D3DLOCKED_RECT rect;
    int res = texture->LockRect(0, &rect, &r, 0);
    if (res != S_OK) {
      #ifdef _DEBUG
      printf("Failed to lock texture: %x\n", res);
      #endif
      return { 0, 0, 0, nullptr };
    }
    this->data = rect.pBits;
    
    return { width, height, rect.Pitch, rect.pBits };
  };

  inline void commit() {
    texture->UnlockRect(0);
  };

  void destroy() {
    if (texture) {
      texture->Release();
      texture = nullptr;
    }
    if (data) free(data);
    initialized = false;
  }

  d3d9_texture_t() {
    width = 0;
    height = 0;
    initialized = false;
  }

  ~d3d9_texture_t() {
    destroy();
  };
};