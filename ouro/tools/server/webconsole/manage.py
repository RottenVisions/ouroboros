#!/usr/bin/env python
import os
import sys

from OUROSettings.path import initExtraRootPath
initExtraRootPath()

if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "OUROSettings.settings")

    from django.core.management import execute_from_command_line

    execute_from_command_line(sys.argv)
