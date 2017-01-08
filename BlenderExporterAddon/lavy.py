bl_info = {
    "name": ".lavy exporter",
    "category": "Import-Export",
}

import bpy
import json
import bmesh
import ctypes
from mathutils import Vector
from bpy.props import StringProperty, BoolProperty
from bpy_extras.io_utils import ExportHelper

class Vertex:
    __slots__ = (
        "pos",
        "normal",
        "uv",
        "index",
        "loop_indices"
    )
    
    def __init__(self, mesh, loop):
        self.pos = mesh.vertices[loop.vertex_index].co.freeze()
        self.normal = loop.normal.freeze()
        self.uv = mesh.uv_layers[0].data[loop.index].uv.freeze()
        self.loop_indices = [loop.index]
        self.index = 0

    def __hash__(self):
        return hash((self.pos, self.normal, self.uv))
    
    def __eq__(self, other):
        eq = (
            (self.pos == other.pos) and
            (self.normal == other.normal) and
            (self.uv == other.uv)
        )

        if eq:
            indices = self.loop_indices + other.loop_indices
            self.loop_indices = indices
            other.loop_indices = indices

        return eq

class LavyExporter(bpy.types.Operator, ExportHelper):
    bl_idname = "export_mesh.lavy"
    bl_label = "Export .lavy"
    filename_ext = ".lavy"
    filter_glob = StringProperty(default="*.lavy", options={'HIDDEN'})
	
    def execute(self, context):
        if bpy.context.active_object.mode!='OBJECT':
            bpy.ops.object.mode_set(mode='OBJECT')
        scene = bpy.context.scene
		
        if len(bpy.context.selected_objects)>0:
            exporting_obj = bpy.context.selected_objects[0]
            mesh = exporting_obj.data

            export_json = open(self.filepath + ".json", "w")
            export_bin = open(self.filepath + ".bin", "wb")

            mesh.calc_normals_split()
            mesh.calc_tessface()

            verts = { Vertex(mesh, loop) : 0 for loop in mesh.loops }.keys()
            verts_count = len(verts)
            offset = 0

            for i, vert in enumerate(verts):
                vert.index = i
                for pos in vert.pos:
                    export_bin.write(ctypes.c_float(pos))
                for normal in vert.normal:
                    export_bin.write(ctypes.c_float(normal))
                uv_x = vert.uv.x
                uv_y = vert.uv.y
                export_bin.write(ctypes.c_float(uv_x))
                export_bin.write(ctypes.c_float(1 - uv_y))
                offset += 32

            vertices_byte_length = offset
            vert_dict = { i : v for v in verts for i in v.loop_indices}

            for poly in mesh.polygons:
                indices = [vert_dict[i].index for i in poly.loop_indices]
                if len(indices) == 3:
                    for i in indices:
                        c_i = ctypes.c_uint32(i)
                        export_bin.write(c_i)
                        offset += 4
                elif len(indices) > 3:
                    for i in range(len(indices) - 2):
                        c_i1 = ctypes.c_uint32(indices[-1])
                        c_i2 = ctypes.c_uint32(indices[i])
                        c_i3 = ctypes.c_uint32(indices[i + 1])
                        export_bin.write(c_i1)
                        export_bin.write(c_i2)
                        export_bin.write(c_i3)
                        offset += 12
            
            indices_byte_length = offset - vertices_byte_length

            j_data = json.dumps(
                {
                    'vertices': {
                        'byteLength': vertices_byte_length,
                        'byteOffset': 0
                    },
                    'indices': {
                        'byteLength': indices_byte_length,
                        'byteOffset': vertices_byte_length
                    }
                }
            )

            export_json.write(j_data)

            export_json.close()
            export_bin.close()
        return {'FINISHED'}

def menu_export(self, context):
    self.layout.operator(LavyExporter.bl_idname, text="Lavy")

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_export)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
    register()