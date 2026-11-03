import json

from django.http import JsonResponse
from django.shortcuts import render
from django.utils import timezone
from django.views.decorators.csrf import csrf_exempt

from .models import SolarReading


@csrf_exempt
def receive_reading(request):
    if request.method != "POST":
        return JsonResponse(
            {"error": "Only POST requests are allowed"},
            status=405
        )

    try:
        data = json.loads(request.body)

        # Convert negative voltage and current values to zero
        voltage = max(0.0, float(data["voltage"]))
        current = max(0.0, float(data["current"]))

        # Calculate power using corrected values
        power = voltage * current

        # Save the reading in the database
        reading = SolarReading.objects.create(
            voltage=voltage,
            current=current,
            power=power,
        )

        return JsonResponse({
            "message": "Reading saved successfully",
            "id": reading.id,
            "voltage": reading.voltage,
            "current": reading.current,
            "power": reading.power,
        })

    except (ValueError, TypeError, KeyError, json.JSONDecodeError):
        return JsonResponse(
            {
                "error": "Invalid reading data. Provide numeric voltage and current values."
            },
            status=400
        )

def dashboard(request):
    latest = SolarReading.objects.order_by("-timestamp", "-id").first()
    return render(request, "readings/dashboard.html", {"latest": latest})


def readings_data(request):
    if request.method != "GET":
        return JsonResponse(
            {"error": "Only GET requests are allowed"},
            status=405
        )

    # Retrieve only the latest 50 records from the database
    readings = list(
        SolarReading.objects.order_by("-timestamp", "-id")[:50]
    )

    # Reverse them so the oldest of these 50 appears first
    readings.reverse()

    data = [
        {
            "id": reading.id,
            "voltage": reading.voltage,
            "current": reading.current,
            "power": reading.power,
            "timestamp": timezone.localtime(
                reading.timestamp
            ).isoformat(),
        }
        for reading in readings
    ]

    latest = readings[-1] if readings else None

    latest_data = None

    if latest:
        latest_data = {
            "voltage": latest.voltage,
            "current": latest.current,
            "power": latest.power,
            "timestamp": timezone.localtime(
                latest.timestamp
            ).isoformat(),
        }

    return JsonResponse({
        "readings": data,
        "latest": latest_data
    })