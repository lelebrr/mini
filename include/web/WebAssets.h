#ifndef WEB_ASSETS_H
#define WEB_ASSETS_H

// WebUI do Mini Lele - 100% autocontida (sem CDN externo), funciona offline
// no modo AP. O HTML fica em WebAssets.cpp (nao em header!) para nao inflar
// cada translation unit que incluir este header. Declaracao externa aqui.

extern const char index_html[];

#endif
