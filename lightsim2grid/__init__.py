__version__ = "0.3.0"

__all__ = ["compute_powerflow", "newtonpf"]
try:
    from lightsim2grid.LightSimBackend import LightSimBackend
    __all__.append("LightSimBackend")
except ImportError:
    # grid2op is not installed, the Backend will not be available
    pass
