from django.urls import path
from . import views


urlpatterns = [
    path("", views.dashboard, name="dashboard"),

    path(
        "reading/",
        views.receive_reading,
        name="receive_reading"
    ),

    path(
        "readings/",
        views.readings_data,
        name="readings_data"
    ),
]