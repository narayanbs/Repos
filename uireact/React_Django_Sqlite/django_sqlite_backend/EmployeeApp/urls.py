from django.conf import settings
from django.conf.urls.static import static
from django.urls import path

from .views import SaveFile, departmentApi, employeeApi

urlpatterns = [
    path("department", departmentApi),
    path("department/<int:id>", departmentApi),
    path("employee", employeeApi),
    path("employee/<int:id>", employeeApi),
    path("employee/savefile", SaveFile),
] + static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)
