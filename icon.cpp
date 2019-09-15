#include "icon.h"
#include "cameramanager.h"
#include "resourceinfo.h"

REGISTER(Billboard)

void Billboard::Create()
{
    program = CResourceInfo::Inst()->CreateProgram("icon.vsh","icon.fsh","icon");
}

void Billboard::SetImage(QImage& image)
{
    pTex = new QOpenGLTexture(image);
    pTex->setMinificationFilter(QOpenGLTexture::Nearest);
    pTex->setMagnificationFilter(QOpenGLTexture::Linear);
    pTex->setWrapMode(QOpenGLTexture::Repeat);
}

void Billboard::ForwardRender()
{
    QOpenGLFunctions* gl = QOpenGLContext::currentContext()->functions();

    //gl->glDepthMask(GL_FALSE);
   // gl->glEnable(GL_BLEND);
   // gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program->bind();
    program->setUniformValue("ModelMatrix",modelMatrix);
    program->setUniformValue("ViewMatrix",Camera::Inst()->GetViewMatrix());
    program->setUniformValue("ProjectMatrix",RenderCommon::Inst()->GetProjectMatrix());
    if(billboardType == BILL_Screen)
        program->setUniformValue("bFixZ",true);
    else
        program->setUniformValue("bFixZ",false);

    gl->glActiveTexture(GL_TEXTURE0);
    pTex->bind();
    program->setUniformValue("LightTex", 0);

    RenderCommon::Inst()->GetGeometryEngine()->drawPlane(program);

//    gl->glDisable(GL_BLEND);
//    gl->glDepthMask(GL_TRUE);
}

void Billboard::UpdateLocation()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(position.x,position.y,position.z);
    if(billboardType == BILL_YAxis)
        modelMatrix *= Camera::Inst()->GetYRotation();
    else
        modelMatrix *= Camera::Inst()->GetRotation();

    modelMatrix.scale(scale.x,scale.y,scale.z);

    IT_modelMatrix = modelMatrix;
    IT_modelMatrix = IT_modelMatrix.transposed();
    IT_modelMatrix = IT_modelMatrix.inverted();
}
