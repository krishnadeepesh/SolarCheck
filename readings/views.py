import json

from django.http import JsonResponse
from django.shortcuts import render
from django.utils import timezone
from django.views.decorators.csrf import csrf_exempt

from .models import SolarReading


@csrf_exempt
def receive_reading(request):
    if request.method == "POST":
        try:
            data = json.loads(request.body)

            voltage = float(data["voltage"])
            current = float(data["current"])
            power = voltage * current

            reading = SolarReading.objects.create(
                voltage=voltage,
                current=current,
                power=power
            )

            return JsonResponse({
                "message": "Reading saved successfully",
                "voltage": reading.voltage,
                "current": reading.current,
                "power": reading.power
            })

        except (ValueError, KeyError, json.JSONDecodeError):
            return JsonResponse(
                {"error": "Invalid reading data"},
                status=400
            )

    return JsonResponse(
        {"error": "Only POST requests are allowed"},
        status=405
    )


def dashboard(request):
    readings = SolarReading.objects.order_by("-timestamp")[:10]
    latest = SolarReading.objects.order_by("-timestamp").first()

    return render(
        request,
        "readings/dashboard.html",
        {
            "readings": readings,
            "latest": latest,
        }
    )


def readings_data(request):
    if request.method != "GET":
        return JsonResponse(
            {"error": "Only GET requests are allowed"},
            status=405
        )

    readings = list(
        SolarReading.objects.order_by("-timestamp")[:10]
    )

    data = []

    for reading in reversed(readings):
        data.append({
            "id": reading.id,
            "voltage": reading.voltage,
            "current": reading.current,
            "power": reading.power,
            "timestamp": timezone.localtime(
                reading.timestamp
            ).isoformat()
        })

    latest = readings[0] if readings else None

    return JsonResponse({
        "readings": data,
        "latest": {
            "voltage": latest.voltage,
            "current": latest.current,
            "power": latest.power,
            "timestamp": timezone.localtime(
                latest.timestamp
            ).isoformat()
        } if latest else None
    })