from django.contrib import admin
from django.urls import path, include
from readings import views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('', views.dashboard, name='dashboard'),
    path('api/', include('readings.urls')),
]
