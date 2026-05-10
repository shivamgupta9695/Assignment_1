import platform
import socket
import json
import shutil
import psutil

from abc import ABC, abstractmethod


# ==========================================
# Parent Abstract Class
# ==========================================

class HostInfo(ABC):

    def __init__(self):

        self.hostname = ""
        self.memory = ""
        self.cpu = ""
        self.ip = ""
        self.disk_size = ""

    @abstractmethod
    def get_hardware_info(self):
        pass

    def display_hardware_info(self):

        data = {
            "hostname": self.hostname,
            "memory": self.memory,
            "cpu": self.cpu,
            "ip": self.ip,
            "disk_size": self.disk_size
        }

        print(json.dumps(data, indent=4))


# ==========================================
# Windows Host Class
# ==========================================

class WindowsHost(HostInfo):

    def get_hardware_info(self):

        self.hostname = socket.gethostname()

        self.ip = socket.gethostbyname(
            self.hostname)

        memory_gb = round(
            psutil.virtual_memory().total
            / (1024 ** 3), 2)

        self.memory = f"{memory_gb} GB"

        self.cpu = platform.processor()

        disk = shutil.disk_usage("/")

        disk_gb = round(
            disk.total / (1024 ** 3), 2)

        self.disk_size = f"{disk_gb} GB"


# ==========================================
# Linux Host Class
# ==========================================

class LinuxHost(HostInfo):

    def get_hardware_info(self):

        self.hostname = socket.gethostname()

        self.ip = socket.gethostbyname(
            self.hostname)

        memory_gb = round(
            psutil.virtual_memory().total
            / (1024 ** 3), 2)

        self.memory = f"{memory_gb} GB"

        self.cpu = platform.processor()

        disk = shutil.disk_usage("/")

        disk_gb = round(
            disk.total / (1024 ** 3), 2)

        self.disk_size = f"{disk_gb} GB"